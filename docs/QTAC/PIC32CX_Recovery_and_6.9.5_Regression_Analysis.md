# PIC32CX Transport Recovery & 6.9.5 vs 6.9.8/6.9.9 Regression Investigation

## 1. Executive Summary

During stress and stability testing on PIC32CX automotive boards (e.g., `TAC_PIC32CXAuto` / platform 54), two classes of failures were observed:

1. **Deterministic failures on a 100% healthy PIC32CX board (`6.9.8` / `6.9.9` regression)**:
   Even when the PIC32CX hardware and USB CDC serial port are completely responsive with zero hardware wedge, multi-entry quick commands (`bootToEDL`, `bootToFastboot`, `powerOff`, `powerOn`) and subsequent `setPinState()` calls fail deterministically or corrupt pin sequencing due to premature transaction completion, background queue collision, split `readyRead` frame loss, and an unsynchronized `PIC32CXDevice::open()` handshake.
2. **Transport recovery failure when a command times out (`performTransportReset()`)**:
   When `waitForCompletion()` times out and invokes `resetTransport()`, `TACPIC32CXDriveThread::performTransportReset()` sleeps through queued script delays on the drive thread (exceeding the 15-second `kTransportResetTimeoutMs`), and `drainHandshake()` fails to complete the post-reopen `*CLS` + `*IDN?` handshake when `echo 1` (`*CLS`) receives no reply or when `_serialPort` fails to reopen, leaving the drive thread spinning indefinitely with a null/unusable port.

This document explains why a **100% stable PIC32CX device without any hardware wedge still fails on 6.9.8/6.9.9**, why **6.9.5 appeared to pass previously**, details each root cause, and outlines the complete solution.

---

## 2. Why a 100% Stable PIC32CX Device (No Hardware Hang) Still Fails on 6.9.8 / 6.9.9

Even when the PIC32CX board and serial driver never wedge and answer every serial command within ~15 ms, **6.9.8 / 6.9.9** fails deterministically due to four software defects in the PIC32CX drive thread, protocol coder, and device open path:

### 2.1 Premature `quickCommand()` Return After ~100 ms While the Script Runs for ~10 Seconds in the Background (RC1)
In `TACPIC32CXDriveThread::sendCommandSequence()`, `TACPIC32CXCommand tacCommand(this, this)` was instantiated **inside** the `for` loop over `commandEntries` (unlike `TACPSOCDriveThread::sendCommandSequence()`, which instantiates `TACPSOCCommand` once outside the loop):
```cpp
for (const auto& commandEntry: commandEntries)
{
    {
        TACPIC32CXCommand tacCommand(this, this);
        switch (commandEntry->_commandAction)
        { ... }
    }
    emit progress(kProgressActive);
}
```
Because `~TACCommand()` queues an `End Transaction` frame (`_endTransaction = true`) on destruction, a 10-step script such as `bootToEDL` in `TAC_PIC32CXAuto_54.tcnf`:
1. `logComment "====== bootToEDL start ======"`
2. `sedl 0` (Pin 219 off)
3. `uefi 0` (Pin 16 off)
4. `battery 1` (Pin 4 on)
5. `pedl 1` (Pin 216 on)
6. `delay 900`
7. `battery 0` (Pin 4 off)
8. `delay $edl` (2500 ms)
9. `pedl 0` (Pin 216 off)
10. `logComment "====== bootToEDL finish ======"`

queues **10 separate `End Transaction` frames**—one after every single entry!
- Step #1 of `bootToEDL`, `powerOff`, and `powerOn` is a `logComment`, which does not touch the serial port at all: `TACPIC32CXDriveThread::run()` pops the comment frame, calls `receive()` immediately, and on the very next iteration pops step #1's `End Transaction` frame.
- In `TACPIC32CXDriveThread::receive()`, `if (framePackage->_endTransaction == true)` immediately calls `clearWaitForCompletion()`.
- Consequently, `_AlpacaDevice::quickCommand("bootToEDL")` wakes from `waitForCompletion()` on its very first 100 ms poll (~100 ms total) and returns `completion confirmed` to the caller **before a single pin command has been written to the PIC32CX serial port**.
- Meanwhile, on the drive thread, each `CONF:DIG:ON` pin write takes ~530–1040 ms (because `SerialPort::readAll()` loops with `waitForReadyRead(_timeout)` where `_timeout = 500` ms until timeout), plus `delay 900` and `delay 2500`. Thus, the drive thread continues executing `bootToEDL` in the background for **~10.1 seconds** after `quickCommand(bootToEDL)` has already returned success!

This causes two immediate failures on a healthy board:
1. **Premature caller action & out-of-order command collision**:
   Because `quickCommand(bootToEDL)` returns 10 seconds early, the caller immediately proceeds as if the DUT is already in EDL mode while the board is still toggling `battery` and `pedl`. When the caller issues a follow-up `setPinState(usb0)` or `quickCommand()`, `_commandDepth` is `0` (because `quickCommand` already returned and exited `AlpacaCommandScope`), so the new command is accepted and appended to `_outboundData` behind the still-running `bootToEDL` frames.
   - If one of `bootToEDL`'s 9 remaining `End Transaction` frames is popped while `setPinState()` is waiting in `waitForCompletion()`, `setPinState()`'s `_waitForCompletion` flag is cleared prematurely by `bootToEDL`'s leftover `End Transaction` frame—making `setPinState()` return before its own `CONF:DIG:ON` command is even sent!
2. **False 5200 ms timeout on a healthy board**:
   In **6.9.9**, commit `7d15ad1` added `setWaitForCompletion()` inside `_AlpacaDevice::setPinState()`, which calls `waitForCompletion(0)` with the default **5200 ms** timeout (no `scriptDurationMs` extension). When a caller invokes `setPinState()` while >5.2 seconds of unpopped script pin writes and delays (`delay 900`, `delay 2500`, etc.) from the prematurely-returned `quickCommand` are still draining ahead of it in `_outboundData`, `setPinState()` times out at 5200 ms (`Wait for completion timed out. Board stopped acknowledging after 5200 ms`) even though the PIC32CX board is 100% healthy and actively executing the queue.

### 2.2 Self-Inflicted Frame Loss on Split `readyRead` Chunks in `TACPIC32CXCoder::decode()` (RC5)
Unlike `TACPSOCCoder::decode()`—which accumulates `_recieveBuffer += decodeMe` and only dispatches and clears `_recieveBuffer` once a complete `\r\n`-terminated response containing the status token has arrived—`TACPIC32CXCoder::decode()` had an `else` branch that unconditionally completed the pending frame and cleared `_recieveBuffer` on every single `readSerialData()` call:
```cpp
void TACPIC32CXCoder::decode(const QByteArray& decodeMe)
{
    static const QString kDelimiter("\r\n");
    _recieveBuffer += decodeMe;
    ...
    else if (_recieveBuffer.size() > kValidPIC32CXResponseSize)
    {
        ...
        _frameFunction(QByteArray(), _protocolInterface);
        _recieveBuffer.clear();
    }
    else
    {
        _frameFunction(QByteArray(), _protocolInterface);
        _recieveBuffer.clear();
    }
}
```
- If the PIC32CX board's ~75-byte `*IDN?` response (`\r\nKarussell,Apr  8 2025,08/04/2025,8C:FD:F0:C0:13:8C,54,TDC000035AQ0,10-72521-1410\r\n`) or a command reply arrives split across two USB CDC transfers where the first chunk is $\le 40$ bytes (or before the trailing `\r\n`), the `else` branch fires on chunk #1, calling `frameComplete(QByteArray())` with an empty `frames` list and clearing `_recieveBuffer`.
- For `*IDN?`, `handleVersionResponse()` receives `framePackage->_responses.count() == 0` (so `_platformID` is never parsed), and when chunk #2 arrives on the next `readyRead`, it either completes the *next* command in the queue with stale `*IDN?` bytes or is discarded.
- Furthermore, if a single chunk $> 40$ bytes arrives before its `\r\n` delimiter, `frames.size()` is `1`, and the original code indexed `frames[1]` out of bounds.

### 2.3 Unsynchronized `PIC32CXDevice::open()` Handshake Racing with the Caller's First Command (RC4)
When `PIC32CXDevice::open()` starts `TACPIC32CXDriveThread`:
1. `PIC32CXDevice::open()` only checked `_driveThread->isRunning()`, which becomes `true` as soon as the OS thread starts—before `openSerialDevice()` even opens the COM port or queues `echo 1` (`*CLS`) and `*IDN?`.
2. When `handleVersionResponse()` processed the first `*IDN?` reply, `setupConnected()` queued a **second duplicate `*IDN?`**.
3. Each `*IDN?` in `run()` executes `_serialPort->clear(QSerialPort::AllDirections)` before writing `*IDN?\n`.
4. When a caller opens the device and immediately issues `quickCommand(bootToEDL)` or `setPinState()`, those frames are queued into `_outboundData` while `echo 1` and the two `*IDN?` commands are still in flight, and `_serialPort->clear(QSerialPort::AllDirections)` wipes in-flight bytes.
5. Moreover, `echo 1` (`kPIC32CXClearBufferCommand`) does not produce a response on PIC32CX firmware unless a prior command's unread output happened to be sitting in the buffer. When `echo 1` produces no reply, `ProtocolInterface::_pendingFramePackage` remains stuck on `echo 1` forever (`getNextFramePackage()` returns a null `FramePackage` while `!_pendingFramePackage.isNull()`), completely deadlocking the outbound queue so that `*IDN?` and all subsequent commands are never even written to the serial port!

---

## 3. Why 6.9.5 Could Pass Before, But 6.9.8 / 6.9.9 Fails Now

Between **6.9.5** and **6.9.8 / 6.9.9**, three commits altered the timing and interaction between `waitForCompletion()`, `ProtocolInterface::getNextFramePackage()`, `performTransportReset()`, and `setPinState()`:

### 3.1 Commit `1093368` ("TAC: make quick-command disconnect/close re-entrancy safe", merged in 6.9.8)
In **6.9.5**, `ProtocolInterface::getNextFramePackage()` slept for `framePackage->_delayInMilliSeconds` while holding `_pendingFramePackageMutex` and `_outQueueMutex`, and `performTransportReset()` drained the outbound queue using:
```cpp
_protocolInterface->clearPendingFrame();
while (_protocolInterface->queueSize() > 0)
{
    FramePackage stale = _protocolInterface->getNextFramePackage();
    if (stale.isNull())
        break;
    _protocolInterface->clearPendingFrame();
}
```
Wait-for-completion timeouts in **6.9.5** were extended by `scriptDurationMs` (`0568ef1`), and `1093368` changed `getNextFramePackage()` so that when it pops a delay frame (`_delayInMilliSeconds != 0`), it sleeps **outside** the lock in 50 ms slices (unless `_skipDelays` is set).

However, `performTransportReset()` in `TACPIC32CXDriveThread` still drained the outbound queue by calling `getNextFramePackage()` in a loop **without** setting `_skipDelays` or clearing `_outboundData` directly:
- When a PIC32CX pin or quick command stalls on an early step of a script (before the script's `eDelay` entries have been popped by `run()`), the remaining script entries—including multi-second delays (e.g., `900 ms + 2500 ms` in `bootToEDL`, or up to 20000 ms across longer boot sequences)—are still sitting in `_outboundData`.
- When `waitForCompletion()` times out on the caller thread and calls `resetTransport()`, the caller thread waits up to **15 seconds** (`kTransportResetTimeoutMs = 15000`) on `_resetCondition`.
- On the drive thread, `performTransportReset()` loops over `getNextFramePackage()` to discard stale frames. Every queued delay frame in `_outboundData` causes `getNextFramePackage()` to **sleep for the full delay duration** inside `performTransportReset()`.
- When the remaining queued delays + `kTransportResetSettleMs` (500 ms) + `drainHandshake()` (5000 ms) exceed 15 seconds, the caller thread times out (`TACDriveThread::resetTransport: timed out waiting for drive thread to reset transport`) while the drive thread is still inside `performTransportReset()`.

### 3.2 Commit `7d15ad1` (merged in 6.9.9) + Per-Entry `TACPIC32CXCommand` Scope in `sendCommandSequence`
- In **6.9.5**:
  - `_AlpacaDevice::setPinState()` did **not** arm `setWaitForCompletion()` directly (only `_setCommandState` and `quickCommand` did), and `AlpacaCommandScope` (`1093368`) did not exist.
  - Because `waitForCompletion()` returned as soon as the first entry's `End Transaction` was processed (~100 ms), `quickCommand()` returned success to the caller immediately while the drive thread continued draining the rest of the script in the background (~10.1 s).
  - If the caller waited several seconds after `quickCommand()` (or if a subsequent `setPinState()` did not arm `_waitForCompletion`), the background queue could finish draining without triggering a 5200 ms timeout, so **6.9.5 appeared to pass**.
- In **6.9.8 / 6.9.9**:
  - Commit `7d15ad1` armed `setWaitForCompletion()` inside `_AlpacaDevice::setPinState()` with a 5200 ms timeout, exposing the background queue backlog and `echo 1` pending-frame deadlock deterministically.

### 3.3 Cascading Failure After a Timed-Out `resetTransport()`
Once `resetTransport()` times out or fails on PIC32CX:
1. Inside `performTransportReset()`, `openSerialDevice()` queues `echo 1` (`clearBuffer()`) which expects a serial reply before `_pendingFramePackage` is cleared, and then `performTransportReset()` queues `*IDN?` (`platformID()`). When the freshly reopened port has nounread bytes in its buffer, the PIC32CX firmware does not send a >40-byte response to `echo 1`, or `echo 1` remains pending in `_pendingFramePackage` so `drainHandshake()` never even sends `*IDN?`! After 5000 ms, `drainHandshake()` always fails with `TACPIC32CXDriveThread::performTransportReset: failed (handshake no response)`.
2. When `performTransportReset()` fails, `run()` does **not** call `stopRunning()` (or when `openSerialDevice()` fails on reopen, `_serialPort` is `Q_NULLPTR`). The drive thread remains running (`weAreRunning() == true`, `isRunning() == true`) in an infinite loop!
3. Because `_AlpacaDevice::_alpacaDevices` caches the `AlpacaDevice` instance and `_driveThread != Q_NULLPTR`, every subsequent command queues frames into a wedged or null `_serialPort` drive thread, timing out every time.

---

## 4. Summary of Root Causes Found

| # | Component | Root Cause | Impact |
|---|-----------|------------|--------|
| **RC1** | `TACPIC32CXDriveThread::sendCommandSequence` | Instantiates `TACPIC32CXCommand` **inside** the `for` loop over `commandEntries`. Its destructor queues an `End Transaction` frame after every single step (`eSetPin`, `eLog`, `eDelay`) instead of once after the entire sequence (unlike `TACPSOCDriveThread`). | `receive()` clears `_waitForCompletion` on step #1's `End Transaction` (after ~100 ms on a `logComment`). `quickCommand()` returns ~10 s before the script actually finishes, leaving leftover delays, pin writes, and `End Transaction` frames in `_outboundData` that collide with subsequent commands and cause false 5200 ms `setPinState()` timeouts on healthy boards. |
| **RC2** | `TACPIC32CXDriveThread::performTransportReset` & `ProtocolInterface` | Drains `_outboundData` by calling `getNextFramePackage()` in a loop. Since `1093368`, `getNextFramePackage()` sleeps for `delayMs` on every queued `eDelay` frame. Also does not call `_frameCoder->reset()` to clear `_recieveBuffer`. | If a timeout occurs while `eDelay` frames remain in `_outboundData`, `performTransportReset()` sleeps through all queued script delays on the drive thread, exceeding `kTransportResetTimeoutMs` (15 s) and causing `resetTransport()` to fail. |
| **RC3** | `TACPIC32CXDriveThread::run` & `performTransportReset` | When `performTransportReset()` fails (`openSerialDevice()` fails or `drainHandshake()` returns `false`), `run()` stays in `while (!loopFinished)` forever because `weAreRunning()` is still `true`, and `echo 1` in `openSerialDevice()` blocks `*IDN?` in `drainHandshake()` when silent. | `performTransportReset()` fails with `handshake no response`, the dead drive thread never exits, `isOpen()` / `weAreRunning()` stays `true`, and callers queue commands into a dead session forever. |
| **RC4** | `PIC32CXDevice::open` & `TACPIC32CXDriveThread::setupConnected` | `PIC32CXDevice::open()` only waits for `_driveThread->isRunning()`, returning before `openSerialDevice()` and the `*IDN?` handshake complete. Furthermore, `setupConnected()` queues a **duplicate** `tacCommand.platformID()` (`*IDN?`) after the first `*IDN?` reply. | Callers issuing a command immediately after `open()` race with the initial handshake and `_serialPort->clear(QSerialPort::AllDirections)`; if `openSerialDevice()` or the handshake fails, `PIC32CXDevice::open()` still returns `true`. |
| **RC5** | `TACPIC32CXCoder::decode` | Unconditionally calls `_frameFunction(QByteArray(), _protocolInterface)` and `_recieveBuffer.clear()` on every `readSerialData()` call even when `\r\n` has not arrived yet; also assumes every >40-byte buffer splits on `\r\n` into at least 2 elements (`frames[1]`). | Prematurely completes `_pendingFramePackage` with empty responses if a reply arrives split across two `readyRead` chunks; out-of-bounds crash (`frames[1]`) if a >40-byte chunk has fewer than two `\r\n`-delimited segments. |
| **RC6** | `TACPIC32CXProtocol::frameComplete` | Declares `static QList<QByteArray> frames;` as a function-local `static` shared across all instances and threads. | Data race if multiple PIC32CX instances are active or if a transport reset clears pending state while frames are accumulated. |

---

## 5. Detailed Solution

### 5.1 Hoist `TACPIC32CXCommand` Outside the Loop in `TACPIC32CXDriveThread::sendCommandSequence` (Fixes RC1)
Match `TACPSOCDriveThread::sendCommandSequence`: instantiate a single `TACPIC32CXCommand` scope for the entire `commandEntries` loop so that `End Transaction` is queued **once** at the end of the quick-command sequence:
```cpp
void TACPIC32CXDriveThread::sendCommandSequence(CommandEntries& commandEntries)
{
    setLastCommandDescription(QString("sendCommandSequence(%1 entries)").arg(commandEntries.count()));

    {
        TACPIC32CXCommand tacCommand(this, this);

        for (const auto& commandEntry: commandEntries)
        {
            switch (commandEntry->_commandAction)
            {
            case _CommandEntry::eNotSet:
                break;
            case _CommandEntry::eSetPin:
                tacCommand.setPinState(commandEntry->_pinID, commandEntry->_arguement.toBool());
                break;
            case _CommandEntry::eLog:
                tacCommand.addLogComment(commandEntry->_arguement.toString().toLatin1() + "\n");
                break;
            case _CommandEntry::eDelay:
                tacCommand.addDelay(commandEntry->_arguement.toUInt());
                break;
            case _CommandEntry::eBaseCommand:
                break;
            }

            emit progress(kProgressActive);
        }
    }
}
```

### 5.2 Add `ProtocolInterface::clearOutboundQueue()` Without Sleeping on Delays (Fixes RC2)
Add a dedicated `ProtocolInterface::clearOutboundQueue()` method that clears `_pendingFramePackage`, locks `_outQueueMutex`, clears `_outboundData` in $O(1)$ without executing any `delayMs` sleeps, and calls `_frameCoder->reset()`:
```cpp
void ProtocolInterface::clearOutboundQueue()
{
    clearPendingFrame();

    QMutexLocker lock(&_outQueueMutex);
    _outboundData.clear();

    if (_frameCoder != Q_NULLPTR)
        _frameCoder->reset();
}
```
Use `_protocolInterface->clearOutboundQueue()` in `TACPIC32CXDriveThread::performTransportReset()` instead of looping over `getNextFramePackage()`.

### 5.3 Ensure Reliable Handshake in `openSerialDevice()` / `performTransportReset()` and Stop the Drive Thread on Unrecoverable Reset Failure (Fixes RC3)
1. In `TACPIC32CXDriveThread::performTransportReset()`, retry `openSerialDevice()` up to 3 attempts spaced by `kTransportResetSettleMs` (500 ms) so transient USB CDC teardown delays do not immediately abort recovery.
2. Ensure `drainHandshake()` cannot be deadlocked by a silent `echo 1` (`clearBuffer()`) frame sitting in `_pendingFramePackage`, and if `performTransportReset()` still fails (`openSerialDevice()` or `drainHandshake()` returns `false`), call `stopRunning()` (closing and deleting `_serialPort` if open) so `run()` exits cleanly and subsequent commands immediately detect `weAreRunning() == false` (`TAC_DEVICE_INACTIVE`).

### 5.4 Synchronize `PIC32CXDevice::open()` with Initial Handshake Readiness, and Remove Duplicate `*IDN?` in `setupConnected()` (Fixes RC4)
1. In `TACPIC32CXDriveThread::setupConnected()`, remove the redundant second `tacCommand.platformID()` call. The board has already just answered `platformID()` (`*IDN?`) in `handleVersionResponse()`, so queuing a second unawaited `*IDN?` only leaves an extra command and buffer clear in flight right when `deviceConnected()` is emitted.
2. In `PIC32CXDevice::open()`, wait for the drive thread to complete its initial connection handshake (`_driveThread->isConnected()`) or fail fast if the drive thread exits (`!_driveThread->isRunning()`), cleaning up `_driveThread` via `close()` on failure just like `PSOCDevice::open()`.

### 5.5 Harden `TACPIC32CXCoder::decode()` for Split Chunks and Bounds Safety, and Make `TACPIC32CXProtocol::_frames` Per-Instance (Fixes RC5 & RC6)
1. In `TACPIC32CXCoder::decode()`, accumulate `_recieveBuffer += decodeMe` and wait until a complete `\r\n`-terminated line has arrived (`_recieveBuffer.endsWith("\r\n")` or error token) before dispatching frames and clearing `_recieveBuffer`, preventing partial USB CDC `readyRead` chunks from prematurely completing `_pendingFramePackage`.
2. Guard `frames` indexing in `TACPIC32CXCoder::decode()` (`if (frames.size() > 1) ... else if (frames.size() == 1) ...`) so a single-segment response >40 bytes never indexes `frames[1]` out of bounds.
3. Replace `static QList<QByteArray> frames;` in `TACPIC32CXProtocol::frameComplete()` with a per-instance member variable `_frames` on `TACPIC32CXProtocol` (cleared on `reset()` / `frameComplete()`).
