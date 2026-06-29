// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//         copyright notice, this list of conditions and the following
//         disclaimer in the documentation and/or other materials provided
//         with the distribution.
//
//     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
//         contributors may be used to endorse or promote products derived
//         from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <qtac/TACLiteCoder.h>
#include <qtac/TACPSOCCoder.h>
#include <qtac/TACPIC32CXCoder.h>
#include <qtac/TACLiteCommand.h>
#include <qtac/TACPSOCCommand.h>
#include <qtac/TACPIC32CXCommand.h>
#include <qtac/TACCommandHashes.h>
#include <qtac/StringUtilities.h>
#include <qtac/ReceiveInterface.h>
#include <qtac/SendInterface.h>

#include <cassert>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

// ===========================================================================
// Minimal stub helpers
// ===========================================================================

// Collects frames delivered via the FrameCompleteFunc callback.
struct FrameCollector
{
    std::vector<qtac::ByteArray> frames;

    static void onFrame(const qtac::ByteArray& f, qtac::ProtocolInterface*)
    {
        // This is a static callback; use a global to route to the active instance.
        // (We set it before each test that needs it.)
        if (s_active) s_active->frames.push_back(f);
    }

    static FrameCollector* s_active;
};
FrameCollector* FrameCollector::s_active = nullptr;

// Null bad-frame callback — just satisfies the signature.
static void onBadFrame(const qtac::ByteArray&, qtac::ProtocolInterface*) {}

// Helper: install a coder with the FrameCollector callback.
static void setupCoder(qtac::FrameCoder& coder, FrameCollector& collector)
{
    FrameCollector::s_active = &collector;
    coder.setupCallbackFunctions(nullptr, FrameCollector::onFrame, onBadFrame);
}

// ===========================================================================
// Minimal mock SendInterface for command tests
// ===========================================================================

struct RecordedSend
{
    qtac::ByteArray command;
    qtac::Arguments arguments;
    bool            console{false};
    bool            store{false};
    bool            isEndTransaction{false};
    bool            isDelay{false};
    uint32_t        delayMs{0};
};

class MockSender : public qtac::SendInterface
{
public:
    std::vector<RecordedSend> log;
    bool                      _ready{true};

    bool ready() override { return _ready; }

    uint32_t send(const qtac::ByteArray& cmd, const qtac::Arguments& args,
                  bool console, qtac::ReceiveInterface*, bool store) override
    {
        log.push_back({cmd, args, console, store, false, false, 0});
        return 0;
    }

    void addDelay(uint32_t ms, qtac::ReceiveInterface*) override
    {
        RecordedSend r;
        r.isDelay = true;
        r.delayMs = ms;
        log.push_back(r);
    }

    void addLogComment(const qtac::ByteArray&) override {}

    void addEndTransaction(qtac::ReceiveInterface*) override
    {
        RecordedSend r;
        r.isEndTransaction = true;
        log.push_back(r);
    }
};

// Minimal no-op ReceiveInterface so command constructors don't get nullptr.
class MockReceiver : public qtac::ReceiveInterface
{
public:
    void receive(qtac::FramePackage&) override {}
};

// ===========================================================================
// TACLiteCoder — encode
// ===========================================================================

static void test_taclite_encode_setpin()
{
    qtac::TACLiteCoder coder;

    // SetPin with design-pin 16 → bus index 0
    {
        qtac::Arguments args;
        args.push_back(true);
        args.push_back(static_cast<uint32_t>(16));
        auto result = coder.encode(qtac::ByteArray("SetPin"), args);
        assert(result == "0");
    }

    // Design-pin 26 → bus index 8 (first Bus B pin)
    {
        qtac::Arguments args;
        args.push_back(false);
        args.push_back(static_cast<uint32_t>(26));
        auto result = coder.encode(qtac::ByteArray("SetPin"), args);
        assert(result == "8");
    }

    // Design-pin 48 → bus index 24 (first Bus D pin)
    {
        qtac::Arguments args;
        args.push_back(true);
        args.push_back(static_cast<uint32_t>(48));
        auto result = coder.encode(qtac::ByteArray("SetPin"), args);
        assert(result == "24");
    }

    // Unknown design-pin → 0xFFFFFFF as string
    {
        qtac::Arguments args;
        args.push_back(true);
        args.push_back(static_cast<uint32_t>(999));
        auto result = coder.encode(qtac::ByteArray("SetPin"), args);
        assert(result == std::to_string(0xFFFFFFF).c_str());
    }

    // Non-SetPin command → "Invalid"
    {
        auto result = coder.encode(qtac::ByteArray("Version"), qtac::Arguments());
        assert(result == "Invalid");
    }
}

static void test_taclite_encode_wrong_command()
{
    qtac::TACLiteCoder coder;
    auto result = coder.encode(qtac::ByteArray("getname"), qtac::Arguments());
    assert(result == "Invalid");
}

static void test_taclite_decode_noop()
{
    // decode() is a no-op — calling it must not crash and must deliver no frames
    qtac::TACLiteCoder coder;
    FrameCollector collector;
    setupCoder(coder, collector);

    coder.decode(qtac::ByteArray("anything"));
    assert(collector.frames.empty());
}

static void test_taclite_reset()
{
    qtac::TACLiteCoder coder;
    coder.reset(); // must not crash
}

// ===========================================================================
// TACPSOCCoder — encode
// ===========================================================================

static void test_tacpsoc_encode_known_commands()
{
    qtac::TACPSOCCoder coder;

    auto enc = [&](const char* cmd, qtac::Arguments args = {}) {
        return coder.encode(qtac::ByteArray(cmd), args);
    };

    // Zero-argument commands end with \r
    assert(enc("Version") == "version\r");
    assert(enc("Get Name") == "getname\r");
    assert(enc("Get UUID") == "sys getFSUUID\r");
    assert(enc("Get Platform ID") == "getboardid\r");
    assert(enc("Get Reset Count") == "getresetcount\r");
    assert(enc("Clear Reset Count") == "clearresetcount\r");
    assert(enc("I2C Read Register Value") == "i2c receive\r");

    // Set Name
    {
        qtac::Arguments args;
        args.push_back(std::string("MyBoard"));
        assert(enc("Set Name", args) == "setname MyBoard\r");
    }

    // I2C Read Register
    {
        qtac::Arguments args;
        args.push_back(std::string("0x50 0x01"));
        assert(enc("I2C Read Register", args) == "i2c readRegisterBytes 0x50 0x01 1\r");
    }

    // I2C Write Register
    {
        qtac::Arguments args;
        args.push_back(std::string("0x50 0x02 0xFF"));
        assert(enc("I2C Write Register", args) == "i2c writeByte 0x50 0x02 0xFF\r");
    }

    // SetPin: state=true(1), pin=53
    {
        qtac::Arguments args;
        args.push_back(true);
        args.push_back(static_cast<uint32_t>(53));
        assert(enc("SetPin", args) == "pin 1 53\r");
    }

    // SetPin: state=false(0), pin=36
    {
        qtac::Arguments args;
        args.push_back(false);
        args.push_back(static_cast<uint32_t>(36));
        assert(enc("SetPin", args) == "pin 0 36\r");
    }
}

static void test_tacpsoc_encode_unknown_appends_cr()
{
    qtac::TACPSOCCoder coder;
    // Unknown command — returned as-is but with trailing \r appended
    auto result = coder.encode(qtac::ByteArray("custom"), qtac::Arguments());
    assert(result == "custom\r");
}

static void test_tacpsoc_encode_already_has_cr()
{
    qtac::TACPSOCCoder coder;
    // A command that already ends with \r must not get a second one
    auto result = coder.encode(qtac::ByteArray("custom\r"), qtac::Arguments());
    assert(result == "custom\r");
    assert(result.size() == 8);
}

// ===========================================================================
// TACPSOCCoder — decode
// ===========================================================================

static void test_tacpsoc_decode_basic()
{
    qtac::TACPSOCCoder coder;
    FrameCollector     collector;
    setupCoder(coder, collector);

    // Feed a response that contains the PSOC command prompt "CMD >> "
    // Format: echo of command + \r\n + response + \r\n + prompt
    qtac::ByteArray input("getboardid\r\n12345\r\nCMD >> ");
    coder.decode(input);

    // Should deliver: "getboardid", "12345", then pad to 3, then empty sentinel
    // collector.frames will be: [getboardid, 12345, <pad>, <sentinel>]
    // The sentinel is an empty ByteArray.
    assert(!collector.frames.empty());
    // Last frame is the empty sentinel
    assert(collector.frames.back().isEmpty());
    // At least 3 content frames + sentinel = 4 minimum
    assert(collector.frames.size() >= 4);
}

static void test_tacpsoc_decode_not_recognized()
{
    qtac::TACPSOCCoder coder;
    FrameCollector     collector;
    setupCoder(coder, collector);

    // Feed the "not recognized" string — should also trigger frame delivery
    coder.decode(qtac::ByteArray("CMD: Command not recognized.\r\nCMD >> "));
    assert(!collector.frames.empty());
    assert(collector.frames.back().isEmpty());
}

static void test_tacpsoc_decode_accumulates()
{
    qtac::TACPSOCCoder coder;
    FrameCollector     collector;
    setupCoder(coder, collector);

    // Split the prompt across two decode calls — no frames until prompt arrives
    coder.decode(qtac::ByteArray("response\r\n"));
    assert(collector.frames.empty()); // no prompt yet

    coder.decode(qtac::ByteArray("CMD >> "));
    assert(!collector.frames.empty()); // now it fires
}

static void test_tacpsoc_decode_reset_clears_buffer()
{
    qtac::TACPSOCCoder coder;
    FrameCollector     collector;
    setupCoder(coder, collector);

    coder.decode(qtac::ByteArray("partial"));
    assert(collector.frames.empty());

    coder.reset();

    // After reset, the partial buffer should be gone; feeding just the
    // prompt should not produce the old "partial" token as a frame.
    coder.decode(qtac::ByteArray("CMD >> "));
    // All frames should be padding/sentinel, none should equal "partial"
    for (const auto& f : collector.frames)
        assert(f != "partial");
}

// ===========================================================================
// TACPIC32CXCoder — encode
// ===========================================================================

static void test_tacpic32cx_encode_setpin()
{
    qtac::TACPIC32CXCoder coder;

    // SetPin: state=true, pin=4  → pin string "004" (padded to 3 chars)
    {
        qtac::Arguments args;
        args.push_back(true);
        args.push_back(static_cast<uint32_t>(4));
        auto result = coder.encode(qtac::ByteArray("CONF:DIG:ON"), args);
        // "CONF:DIG:ON 1 (@004)\n"
        assert(result == "CONF:DIG:ON 1 (@004)\n");
    }

    // SetPin: state=false, pin=21 → pin string "021" (padded)
    {
        qtac::Arguments args;
        args.push_back(false);
        args.push_back(static_cast<uint32_t>(21));
        auto result = coder.encode(qtac::ByteArray("CONF:DIG:ON"), args);
        assert(result == "CONF:DIG:ON 0 (@021)\n");
    }

    // SetPin: pin=100 → "100" (already 3 chars, no padding)
    {
        qtac::Arguments args;
        args.push_back(true);
        args.push_back(static_cast<uint32_t>(100));
        auto result = coder.encode(qtac::ByteArray("CONF:DIG:ON"), args);
        assert(result == "CONF:DIG:ON 1 (@100)\n");
    }
}

static void test_tacpic32cx_encode_other_commands()
{
    qtac::TACPIC32CXCoder coder;

    // Other commands just get a trailing \n
    auto v = coder.encode(qtac::ByteArray("Version"), qtac::Arguments());
    assert(v == "Version\n");

    auto idn = coder.encode(qtac::ByteArray("*IDN?"), qtac::Arguments());
    assert(idn == "*IDN?\n");

    // Already ends with \n — should not double-append
    auto already = coder.encode(qtac::ByteArray("echo 1\n"), qtac::Arguments());
    assert(already == "echo 1\n");
    assert(already.size() == 7);
}

// ===========================================================================
// TACPIC32CXCoder — decode
// ===========================================================================

static void test_tacpic32cx_decode_error_prompt()
{
    qtac::TACPIC32CXCoder coder;
    FrameCollector        collector;
    setupCoder(coder, collector);

    // Error prompt → empty sentinel only
    coder.decode(qtac::ByteArray("Error!!! port > something"));
    assert(collector.frames.size() == 1);
    assert(collector.frames[0].isEmpty());
}

static void test_tacpic32cx_decode_unknown_command()
{
    qtac::TACPIC32CXCoder coder;
    FrameCollector        collector;
    setupCoder(coder, collector);

    coder.decode(qtac::ByteArray("*** Command Processor: unknown command. ***\r\n"));
    assert(collector.frames.size() == 1);
    assert(collector.frames[0].isEmpty());
}

static void test_tacpic32cx_decode_valid_response()
{
    qtac::TACPIC32CXCoder coder;
    FrameCollector        collector;
    setupCoder(coder, collector);

    // A response larger than kValidPIC32CXResponseSize (40 bytes).
    // Format: command echo \r\n + actual result \r\n + trailing prompt
    // The coder delivers frames[1] (index 1) + empty sentinel.
    std::string payload =
        "CONF:DIG:ON 1 (@004)\r\n"  // frames[0] — echoed command (22 chars)
        "OK\r\n"                     // frames[1] — actual response
        "port > ";                   // frames[2] — trailing prompt text
    // Total > 40 bytes to exceed kValidPIC32CXResponseSize
    assert(payload.size() > 40);

    coder.decode(qtac::ByteArray(payload.c_str()));

    // Should be: frames[1]="OK", then empty sentinel
    assert(collector.frames.size() == 2);
    assert(collector.frames[0] == "OK");
    assert(collector.frames[1].isEmpty());
}

static void test_tacpic32cx_decode_incomplete()
{
    qtac::TACPIC32CXCoder coder;
    FrameCollector        collector;
    setupCoder(coder, collector);

    // Buffer <= kValidPIC32CXResponseSize and no error prompt →
    // sends empty sentinel and clears.
    coder.decode(qtac::ByteArray("short"));
    assert(collector.frames.size() == 1);
    assert(collector.frames[0].isEmpty());
}

// ===========================================================================
// TACCommandHashes — verify hash constants match arrayHash of the actual strings
// ===========================================================================

static void test_command_hash_constants()
{
    // These constants are used for dispatch in the coders. A regression in
    // arrayHash() or a typo in the constant would silently break encoding.
    assert(arrayHash(qtac::ByteArray("Version"))           == kVersionCommandHash);
    assert(arrayHash(qtac::ByteArray("Get Name"))          == kGetNameCommandHash);
    assert(arrayHash(qtac::ByteArray("Set Name"))          == kSetNameCommandHash);
    assert(arrayHash(qtac::ByteArray("Get UUID"))          == kGetUUIDCommandHash);
    assert(arrayHash(qtac::ByteArray("Get Platform ID"))   == kGetPlatformIDCommandHash);
    assert(arrayHash(qtac::ByteArray("Get Reset Count"))   == kGetResetCountCommandHash);
    assert(arrayHash(qtac::ByteArray("Clear Reset Count")) == kClearResetCountCommandHash);
    assert(arrayHash(qtac::ByteArray("I2C Read Register")) == kI2CReadRegisterCommandHash);
    assert(arrayHash(qtac::ByteArray("I2C Read Register Value")) == kI2CReadRegisterValueCommandHash);
    assert(arrayHash(qtac::ByteArray("I2C Write Register"))== kI2CWriteRegisterCommandHash);
    assert(arrayHash(qtac::ByteArray("SetPin"))            == kSetPinCommandHash);

    // PIC32CX hashes
    assert(arrayHash(qtac::ByteArray("echo 1"))            == kPIC32CXClearBufferHash);
    assert(arrayHash(qtac::ByteArray("*IDN?"))             == kPIC32CXVersionCommandHash);
    assert(arrayHash(qtac::ByteArray("CONF:DIG:ON"))       == kPIC32CXSetPinCommandHash);
}

// ===========================================================================
// TACLiteCommand — mock send verification
// ===========================================================================

static void test_taclite_command_version()
{
    MockSender   sender;
    MockReceiver receiver;
    {
        qtac::TACLiteCommand cmd(&sender, &receiver);
        cmd.version();
    } // destructor calls addEndTransaction

    assert(sender.log.size() == 2);
    assert(sender.log[0].command == "Version");
    assert(sender.log[1].isEndTransaction);
}

static void test_taclite_command_setpin()
{
    MockSender   sender;
    MockReceiver receiver;
    {
        qtac::TACLiteCommand cmd(&sender, &receiver);
        cmd.setPinState(16, true);
    }

    assert(sender.log[0].command == "SetPin");
    assert(sender.log[0].arguments.size() == 2);
    assert(std::get<bool>(sender.log[0].arguments[0]) == true);
    assert(std::get<uint32_t>(sender.log[0].arguments[1]) == 16u);
}

static void test_taclite_command_battery()
{
    MockSender   sender;
    MockReceiver receiver;
    {
        qtac::TACLiteCommand cmd(&sender, &receiver);
        cmd.battery(false);
    }

    assert(sender.log[0].command == "Battery");
    assert(std::get<bool>(sender.log[0].arguments[0]) == false);
}

static void test_taclite_command_setname()
{
    MockSender   sender;
    MockReceiver receiver;
    {
        qtac::TACLiteCommand cmd(&sender, &receiver);
        cmd.setName(qtac::ByteArray("TestBoard"));
    }

    assert(sender.log[0].command == "Set Name");
    assert(std::get<std::string>(sender.log[0].arguments[0]) == "TestBoard");
}

// ===========================================================================
// TACPSOCCommand — mock send verification
// ===========================================================================

static void test_tacpsoc_command_version()
{
    MockSender   sender;
    MockReceiver receiver;
    {
        qtac::TACPSOCCommand cmd(&sender, &receiver);
        cmd.version();
    }

    assert(sender.log[0].command == "Version");
    assert(sender.log.back().isEndTransaction);
}

static void test_tacpsoc_command_setpin()
{
    MockSender   sender;
    MockReceiver receiver;
    {
        qtac::TACPSOCCommand cmd(&sender, &receiver);
        cmd.setPinState(53, true);
    }

    assert(sender.log[0].command == "SetPin");
    assert(std::get<bool>(sender.log[0].arguments[0]) == true);
    assert(std::get<uint32_t>(sender.log[0].arguments[1]) == 53u);
}

static void test_tacpsoc_command_i2c_read()
{
    MockSender   sender;
    MockReceiver receiver;
    {
        qtac::TACPSOCCommand cmd(&sender, &receiver);
        cmd.i2CReadRegister(0x50, 0x10);
    }

    // Should produce: I2C Read Register, delay 500ms, I2C Read Register Value, EndTransaction
    assert(sender.log[0].command == "I2C Read Register");
    assert(std::get<std::string>(sender.log[0].arguments[0]) == "0x50 0x10");
    assert(sender.log[1].isDelay);
    assert(sender.log[1].delayMs == 500);
    assert(sender.log[2].command == "I2C Read Register Value");
    assert(sender.log.back().isEndTransaction);
}

static void test_tacpsoc_command_i2c_write()
{
    MockSender   sender;
    MockReceiver receiver;
    {
        qtac::TACPSOCCommand cmd(&sender, &receiver);
        cmd.i2CWriteRegister(0x50, 0x10, 0xAB);
    }

    assert(sender.log[0].command == "I2C Write Register");
    assert(std::get<std::string>(sender.log[0].arguments[0]) == "0x50 0x10 0xAB");
}

static void test_tacpsoc_command_i2c_invalid_args()
{
    MockSender   sender;
    MockReceiver receiver;
    qtac::TACPSOCCommand cmd(&sender, &receiver);

    bool threw = false;
    try { cmd.i2CReadRegister(0x100, 0x00); }
    catch (const std::out_of_range&) { threw = true; }
    assert(threw);

    threw = false;
    try { cmd.i2CWriteRegister(0x00, 0x100, 0x00); }
    catch (const std::out_of_range&) { threw = true; }
    assert(threw);

    threw = false;
    try { cmd.i2CWriteRegister(0x00, 0x00, 0x100); }
    catch (const std::out_of_range&) { threw = true; }
    assert(threw);
}

// ===========================================================================
// TACPIC32CXCommand — mock send verification
// ===========================================================================

static void test_tacpic32cx_command_setpin()
{
    MockSender   sender;
    MockReceiver receiver;
    {
        qtac::TACPIC32CXCommand cmd(&sender, &receiver);
        cmd.setPinState(4, true);
    }

    assert(sender.log[0].command == "CONF:DIG:ON");
    assert(std::get<bool>(sender.log[0].arguments[0]) == true);
    assert(std::get<uint32_t>(sender.log[0].arguments[1]) == 4u);
    assert(sender.log.back().isEndTransaction);
}

static void test_tacpic32cx_command_clear_buffer()
{
    MockSender   sender;
    MockReceiver receiver;
    {
        qtac::TACPIC32CXCommand cmd(&sender, &receiver);
        cmd.clearBuffer();
    }

    assert(sender.log[0].command == "echo 1");
}

static void test_tacpic32cx_command_platform_id()
{
    MockSender   sender;
    MockReceiver receiver;
    {
        qtac::TACPIC32CXCommand cmd(&sender, &receiver);
        cmd.platformID();
    }

    assert(sender.log[0].command == "*IDN?");
}

// ===========================================================================
// main
// ===========================================================================

int main()
{
    // TACLiteCoder
    test_taclite_encode_setpin();
    test_taclite_encode_wrong_command();
    test_taclite_decode_noop();
    test_taclite_reset();

    // TACPSOCCoder encode
    test_tacpsoc_encode_known_commands();
    test_tacpsoc_encode_unknown_appends_cr();
    test_tacpsoc_encode_already_has_cr();

    // TACPSOCCoder decode
    test_tacpsoc_decode_basic();
    test_tacpsoc_decode_not_recognized();
    test_tacpsoc_decode_accumulates();
    test_tacpsoc_decode_reset_clears_buffer();

    // TACPIC32CXCoder encode
    test_tacpic32cx_encode_setpin();
    test_tacpic32cx_encode_other_commands();

    // TACPIC32CXCoder decode
    test_tacpic32cx_decode_error_prompt();
    test_tacpic32cx_decode_unknown_command();
    test_tacpic32cx_decode_valid_response();
    test_tacpic32cx_decode_incomplete();

    // Hash constants
    test_command_hash_constants();

    // TACLiteCommand
    test_taclite_command_version();
    test_taclite_command_setpin();
    test_taclite_command_battery();
    test_taclite_command_setname();

    // TACPSOCCommand
    test_tacpsoc_command_version();
    test_tacpsoc_command_setpin();
    test_tacpsoc_command_i2c_read();
    test_tacpsoc_command_i2c_write();
    test_tacpsoc_command_i2c_invalid_args();

    // TACPIC32CXCommand
    test_tacpic32cx_command_setpin();
    test_tacpic32cx_command_clear_buffer();
    test_tacpic32cx_command_platform_id();

    std::printf("All coder/command tests passed!\n");
    return 0;
}
