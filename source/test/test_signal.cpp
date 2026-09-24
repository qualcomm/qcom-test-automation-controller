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

#include <qtac/Signal.h>
#include <cassert>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

// ===== Basic fire =====

static void test_basic_fire()
{
    qtac::Signal<int> sig;

    int received = -1;
    sig.connect([&](int v) { received = v; });
    sig(42);
    assert(received == 42);

    sig(99);
    assert(received == 99);
}

// ===== No subscribers =====

static void test_no_subscribers()
{
    qtac::Signal<int> sig;
    // Must not crash with no subscribers
    sig(1);
    assert(!(bool)sig);
}

// ===== operator bool =====

static void test_operator_bool()
{
    qtac::Signal<> sig;
    assert(!(bool)sig);

    int id = sig.connect([] {});
    assert((bool)sig);

    sig.disconnect(id);
    assert(!(bool)sig);
}

// ===== Multiple subscribers =====

static void test_multiple_subscribers()
{
    qtac::Signal<int> sig;

    int sum = 0;
    sig.connect([&](int v) { sum += v; });
    sig.connect([&](int v) { sum += v * 2; });

    sig(10);
    assert(sum == 30); // 10 + 20
}

// ===== Disconnect =====

static void test_disconnect()
{
    qtac::Signal<int> sig;

    int a = 0, b = 0;
    int idA = sig.connect([&](int v) { a += v; });
    int idB = sig.connect([&](int v) { b += v; });

    sig(5);
    assert(a == 5 && b == 5);

    sig.disconnect(idA);
    sig(3);
    assert(a == 5);  // no longer fired
    assert(b == 8);  // still fired

    sig.disconnect(idB);
    sig(10);
    assert(b == 8);  // no longer fired
}

// ===== Disconnect non-existent ID is a no-op =====

static void test_disconnect_invalid()
{
    qtac::Signal<int> sig;
    sig.connect([](int) {});
    sig.disconnect(9999); // must not crash
    sig(1);               // must still fire remaining subscriber
}

// ===== Zero-argument signal =====

static void test_zero_args()
{
    qtac::Signal<> sig;
    int count = 0;
    sig.connect([&] { ++count; });
    sig();
    sig();
    assert(count == 2);
}

// ===== Multi-argument signal =====

static void test_multi_args()
{
    qtac::Signal<int, double, std::string> sig;

    int    gi = 0;
    double gd = 0.0;
    std::string gs;

    sig.connect([&](int i, double d, std::string s) {
        gi = i; gd = d; gs = std::move(s);
    });

    sig(7, 3.14, std::string("hello"));
    assert(gi == 7);
    assert(gd > 3.13 && gd < 3.15);
    assert(gs == "hello");
}

// ===== Lifetime-safe connect (weak_ptr) =====

static void test_weak_ptr_lifetime()
{
    qtac::Signal<int> sig;

    int fired = 0;

    {
        auto owner = std::make_shared<int>(0);
        sig.connect(std::weak_ptr<int>(owner), [&](int v) { fired += v; });

        sig(10);
        assert(fired == 10);

        // owner goes out of scope here — slot should auto-disconnect
    }

    // After owner is gone the slot must not fire
    sig(5);
    assert(fired == 10);  // no change
}

// ===== Weak_ptr connect alongside regular connect =====

static void test_weak_mixed()
{
    qtac::Signal<int> sig;

    int permanent = 0;
    int guarded   = 0;

    sig.connect([&](int v) { permanent += v; });

    {
        auto owner = std::make_shared<int>(42);
        sig.connect(std::weak_ptr<int>(owner), [&](int v) { guarded += v; });

        sig(3);
        assert(permanent == 3 && guarded == 3);
    }

    sig(7);
    assert(permanent == 10); // still fires
    assert(guarded   == 3);  // stopped after owner destroyed
}

// ===== IDs are unique and monotonically increasing =====

static void test_unique_ids()
{
    qtac::Signal<> sig;
    int id1 = sig.connect([] {});
    int id2 = sig.connect([] {});
    int id3 = sig.connect([] {});
    assert(id1 < id2 && id2 < id3);
}

// ===== Signal is reusable after all disconnects =====

static void test_reuse_after_disconnect()
{
    qtac::Signal<int> sig;

    int v = 0;
    int id = sig.connect([&](int x) { v = x; });
    sig(1);
    assert(v == 1);

    sig.disconnect(id);
    id = sig.connect([&](int x) { v = x + 100; });
    sig(2);
    assert(v == 102);
}

int main()
{
    test_basic_fire();
    test_no_subscribers();
    test_operator_bool();
    test_multiple_subscribers();
    test_disconnect();
    test_disconnect_invalid();
    test_zero_args();
    test_multi_args();
    test_weak_ptr_lifetime();
    test_weak_mixed();
    test_unique_ids();
    test_reuse_after_disconnect();

    std::printf("All Signal tests passed!\n");
    return 0;
}
