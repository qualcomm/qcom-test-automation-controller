# PIC32CX Transport Recovery & 6.9.5 vs 6.9.8/6.9.9 Regression Investigation

## 1. Executive Summary

During stress and stability testing on PIC32CX automotive boards (e.g., `TAC_PIC32CXAuto` / platform 54), two classes of failures were observed:

1. **Quick-command timeout & recovery failure introduced after 6.9.5 (`6.9.8` / `6.9.9` regression)**:
   Multi-entry quick commands (`bootToEDL`, `bootToFastboot`, `powerOff`, `powerOn`, etc.) and `performTransportReset()` fail when a PIC32CX serial transaction times out mid-sequence, causing `TACDriveThread::resetTransport` to time out after 15 seconds and leaving the serial port unusable (`QSerialPort` error 8 / `PermissionError` on subsequent opens).
2. **Pre-existing PIC32CX protocol & framing defects**:
   Even when `performTransportReset()` is invoked on an idle queue, `TACPIC32CXDriveThread::sendCommandSequence` queues an `End Transaction` after **every single step** of a script instead of once at the end of the sequence, `TACPIC32CXDriveThread::open()` returns before the initial `*CLS` + `*IDN?` handshake completes, and `TACPIC32CXCoder::decode()` can index `frames[1]` out-of-bounds when a chunk exceeds 40 bytes without a `\r\n` delimiter.

This document summarizes why **6.9.5 passed previously while 6.9.8/6.9.9 fails**, details each root cause, and outlines the complete solution.

---

## 2. Why 6.9.5 Could Pass Before, But 6.9.8 / 6.9.9 Fails Now

Between **6.9.5** and **6.9.8 / 6.9.9**, three commits altered the timing and interaction between `waitForCompletion()`, `ProtocolInterface::getNextFramePackage()`, `performTransportReset()`, and `setPinState()`:

### 2.1 Commit `1093368` ("TAC: make quick-command disconnect/close re-entrancy safe", merged in 6.9.8)
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

However, `performTransportReset()` in `TACPIC32CXDriveThread` still drained the outbound queue by calling `getNextFramePackage()` in a loop **without** setting `_skipDelays` or clearing `_outboundData` directly!
- When a PIC32CX pin or quick command stalls on an early step of a script (before the script's `eDelay` entries have been popped by `run()`), the remaining script entries—including multi-second delays (e.g., 5000 ms, 10000 ms, or up to 20000 ms total across a boot sequence)—are still sitting in `_outboundData`.
- When `waitForCompletion()` times out on the caller thread and calls `resetTransport()`, the caller thread waits up to **15 seconds** (`kTransportResetTimeoutMs = 15000`) on `_resetCondition`.
- On the drive thread, `performTransportReset()` loops over `getNextFramePackage()` to discard stale frames. Every queued delay frame in `_outboundData` causes `getNextFramePackage()` to **sleep for the full delay duration** inside `performTransportReset()`!
- When the remaining queued delays + `kTransportResetSettleMs` (500 ms) + `drainHandshake()` (up to 5000 ms) exceed 15 seconds, the caller thread times out (`TACDriveThread::resetTransport: timed out waiting for drive thread to reset transport`) while the drive thread is still inside `performTransportReset()`.

### 2.2 Commit `0568ef1` ("Allow for script delays when waiting on quick commands", merged in 6.9.5) Combined with Per-Entry `TACPIC32CXCommand` Scope in `sendCommandSequence`
In `TACPIC32CXDriveThread::sendCommandSequence()`:
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
`TACPIC32CXCommand`'s destructor (`~TACCommand()`) queues an `End Transaction` frame (`_endTransaction = true`) at the end of **every loop iteration**—after every single `eSetPin`, `eLog`, and `eDelay` entry!
- In `TACPIC32CXDriveThread::receive()`, **the very first `End Transaction` frame** calls `clearWaitForCompletion()`, immediately releasing `waitForCompletion()` on the caller thread after the first script entry is processed, while the remaining 10–15 entries (pins and delays) are still queued in `_outboundData`!
- In **6.9.5**:
  - `_AlpacaDevice::setPinState()` did **not** arm `setWaitForCompletion()` directly (only `_setCommandState` and `quickCommand` did), and `commandInProgress()` / `AlpacaCommandScope` (`1093368`) did not exist.
  - Because `waitForCompletion()` returned as soon as the first entry's `End Transaction` was processed (~50–500 ms), `quickCommand()` returned success to the caller immediately while the drive thread continued draining the rest of the script in the background.
  - If the caller was idle for several seconds after `quickCommand()` (such as waiting for the DUT to enumerate in EDL/Fastboot), the background queue happened to finish draining before the next TAC call, so **6.9.5 appeared to pass**.
- In **6.9.8 / 6.9.9**:
  - Commit `7d15ad1` (6.9.9) added `setWaitForCompletion()` inside `_AlpacaDevice::setPinState()`, and `1093368` (6.9.8) added `AlpacaCommandScope` (`TAC_DEVICE_BUSY`) and `wasRunning && weAreRunning() == false` checks.
  - Because `quickCommand()` returned prematurely after entry #1's `End Transaction`, the caller (e.g., TacService or test script) could immediately issue the next command (or `setPinState` / `open` / `close`) while the drive thread was still executing the previous `quickCommand`'s remaining delays and pin writes in `_outboundData`.
  - Any subsequent command's `setWaitForCompletion()` either:
    1. Had its `_waitForCompletion` flag cleared prematurely by one of the **previous** command's leftover `End Transaction` frames still in the queue (causing out-of-order completion and race conditions on the serial port), or
    2. Sat behind the previous command's unpopped `eDelay` frames in `_outboundData` without their `scriptDurationMs` allowance, causing a 5200 ms `Wait for completion timed out` on a healthy board!

### 2.3 Cascading Failure After a Timed-Out `resetTransport()`
Once `resetTransport()` times out or fails on PIC32CX:
1. When a command times out because the board is mid-delay or a serial write/read stalls, `performTransportReset()` closes and deletes `_serialPort`, waits 500 ms, and calls `openSerialDevice()`.
2. If the USB CDC endpoint (`usbser.sys` on Windows) experienced a transient glitch or the drive thread was still mid-reset when the caller gave up and tried to re-open the device, `QSerialPort::open()` fails with Windows error / `QSerialPort::UnknownError` (error 8, `ERROR_NOT_ENOUGH_MEMORY` / driver buffer exhaustion in `usbser.sys`) or `PermissionError` (because the previous drive thread still holds the COM port open).
3. In `TACPIC32CXDriveThread::run()`, when `performTransportReset()` fails (`_serialPort == Q_NULLPTR`), `run()` does **not** call `stopRunning()`. The drive thread remains running (`weAreRunning() == true`, `isRunning() == true`) in an infinite `msleep(10)` loop with `_serialPort == Q_NULLPTR`!
4. Because `_AlpacaDevice::_alpacaDevices` caches the `AlpacaDevice` instance and `_driveThread != Q_NULLPTR`, every subsequent command queues frames into a dead drive thread with `_serialPort == Q_NULLPTR`, timing out after 5200 ms every time.

---

## 3. Summary of Root Causes Found

| # | Component | Root Cause | Impact |
|---|-----------|------------|--------|
| **RC1** | `TACPIC32CXDriveThread::sendCommandSequence` | Instantiates `TACPIC32CXCommand` **inside** the `for` loop over `commandEntries`. Its destructor queues an `End Transaction` frame after every single step (`eSetPin`, `eLog`, `eDelay`) instead of once after the entire sequence (unlike `TACPSOCDriveThread`). | `receive()` clears `_waitForCompletion` on the **first** step's `End Transaction`. `quickCommand()` returns before steps 2..N execute, leaving leftover delays, pin writes, and `End Transaction` frames in `_outboundData` that corrupt subsequent commands and cause false 5200 ms timeouts in 6.9.8/6.9.9. |
| **RC2** | `TACPIC32CXDriveThread::performTransportReset` & `ProtocolInterface` | Drains `_outboundData` by calling `getNextFramePackage()` in a loop. Since `1093368`, `getNextFramePackage()` sleeps for `delayMs` on every queued `eDelay` frame. Also does not call `_frameCoder->reset()` to clear `_recieveBuffer`. | If a timeout occurs while `eDelay` frames remain in `_outboundData`, `performTransportReset()` sleeps through all queued script delays on the drive thread, exceeding `kTransportResetTimeoutMs` (15 s) and causing `resetTransport()` to fail. |
| **RC3** | `TACPIC32CXDriveThread::run` & `performTransportReset` | When `performTransportReset()` fails to reopen the serial port (`_serialPort == Q_NULLPTR`), `run()` stays in `while (!loopFinished)` doing `msleep(10)` forever because `weAreRunning()` is still `true`. | The dead drive thread never exits, `isOpen()` / `weAreRunning()` stays `true`, and callers queue commands into a null `_serialPort` forever instead of failing fast with `TAC_DEVICE_INACTIVE`. |
| **RC4** | `PIC32CXDevice::open` & `TACPIC32CXDriveThread::run` | `PIC32CXDevice::open()` only waits for `_driveThread->isRunning()`, which becomes true before `openSerialDevice()` and the `*CLS` + `*IDN?` handshake even finish. Furthermore, `setupConnected()` queues a **duplicate** `tacCommand.platformID()` (`*IDN?`) after the first `*IDN?` reply. | Callers issuing a command immediately after `open()` race with the initial `*CLS` and dual `*IDN?` frames; furthermore, if `openSerialDevice()` fails on startup, `isRunning()` can briefly return `true` before `run()` exits, making `PIC32CXDevice::open()` return `true` on a failed open. |
| **RC5** | `TACPIC32CXCoder::decode` | Assumes every response with `_recieveBuffer.size() > 40` (`kValidPIC32CXResponseSize`) splits on `\r\n` into at least 2 elements and unconditionally accesses `frames[1]`. Also does not wait for a complete `\r\n`-terminated frame if TCP/USB-CDC delivers a partial chunk, clearing `_recieveBuffer` on every `readSerialData()` call. | Out-of-bounds crash (`ASSERT failure in QList::operator[]: "index out of range"`) if a >40-byte chunk has fewer than two `\r\n`-delimited segments; dropped responses if an `*IDN?` reply arrives split across two `readyRead` chunks. |
| **RC6** | `TACPIC32CXProtocol::frameComplete` | Declares `static QList<QByteArray> frames;` as a function-local `static` shared across all instances and threads. | Data race if two PIC32CX boards are active or if a reset re-enters `frameComplete`. |

---

## 4. Detailed Solution

### 4.1 Hoist `TACPIC32CXCommand` Outside the Loop in `TACPIC32CXDriveThread::sendCommandSequence` (Fixes RC1)
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

### 4.2 Add `ProtocolInterface::clearOutboundQueue()` Without Sleeping on Delays (Fixes RC2)
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
Use `_protocolInterface->clearOutboundQueue()` in `TACPIC32CXDriveThread::performTransportReset()` (and other drive threads' transport resets) instead of looping over `getNextFramePackage()`.

### 4.3 Stop the Drive Thread if `performTransportReset()` Leaves `_serialPort == Q_NULLPTR`, and Retry Transient Reopen Errors (Fixes RC3)
1. In `TACPIC32CXDriveThread::performTransportReset()`, if `openSerialDevice()` fails on the first attempt after `kTransportResetSettleMs`, retry with backoff (e.g., up to 3 attempts spaced by `kTransportResetSettleMs`) so transient `usbser.sys` teardown delays do not permanently kill the session.
2. If `performTransportReset()` still fails, call `stopRunning()` so `run()` exits cleanly instead of spinning in `msleep(10)` with a null `_serialPort`, and so subsequent commands immediately detect `weAreRunning() == false` (`TAC_DEVICE_INACTIVE`).

### 4.4 Synchronize `PIC32CXDevice::open()` with Serial Open & Initial Handshake, and Remove Duplicate `*IDN?` in `setupConnected()` (Fixes RC4)
1. In `TACPIC32CXDriveThread::setupConnected()`, remove the redundant second `tacCommand.platformID()` call. The board has already just answered `platformID()` (`*IDN?`) in `handleVersionResponse()`, so queuing a second unawaited `*IDN?` only leaves an extra command in flight right when `deviceConnected()` is emitted.
2. In `PIC32CXDevice::open()`, wait for `_driveThread->weAreRunning()` and initial connection readiness (or `errorOnOpen` / thread exit) rather than `QThread::isRunning()`, ensuring `open()` returns `false` if `openSerialDevice()` fails and does not return to the caller while the initial `*CLS` + `*IDN?` handshake is still in flight.

### 4.5 Harden `TACPIC32CXCoder::decode()` and `TACPIC32CXProtocol::frameComplete()` (Fixes RC5 & RC6)
1. Guard `frames` indexing in `TACPIC32CXCoder::decode()` (`if (frames.size() > 1) ... else if (frames.size() == 1) ...`) so a single-segment response >40 bytes never indexes `frames[1]` out of bounds.
2. Replace `static QList<QByteArray> frames;` in `TACPIC32CXProtocol::frameComplete()` with a member variable `_frames` on `TACPIC32CXProtocol` (cleared in `TACPIC32CXCoder::reset()` / `clearOutboundQueue()`).
