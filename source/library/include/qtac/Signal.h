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

// Qt-free multi-subscriber signal template.
// Replaces Qt signals/slots for the qtac library.
//
// Usage:
//   qtac::Signal<int, bool> onFoo;
//
//   // Connect — returns an ID for later disconnect
//   int id = onFoo.connect([](int x, bool y) { ... });
//
//   // Lifetime-safe connect — auto-disconnects when owner is destroyed
//   int id = onFoo.connect(weakPtr, [](int x, bool y) { ... });
//
//   // Fire all live subscribers
//   onFoo(42, true);
//
//   // Disconnect one subscriber
//   onFoo.disconnect(id);

#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

namespace qtac {

template<typename... Args>
class Signal
{
public:
    using Slot = std::function<void(Args...)>;

    // Connect a plain callable. Returns a connection ID for disconnect().
    int connect(Slot slot)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        const int id = ++_nextID;
        _slots.push_back({id, std::nullopt, std::move(slot)});
        return id;
    }

    // Lifetime-safe connect. The slot is silently dropped when the weak_ptr
    // expires, so no explicit disconnect is needed if the owner is destroyed.
    template<typename T>
    int connect(std::weak_ptr<T> weak, Slot slot)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        const int id = ++_nextID;
        // Wrap the slot: check the weak_ptr before calling.
        auto guard = [weak, slot = std::move(slot)](Args... args) {
            if (!weak.expired())
                slot(std::forward<Args>(args)...);
        };
        _slots.push_back({id, weak, std::move(guard)});
        return id;
    }

    // Disconnect a previously connected slot by its ID.
    void disconnect(int id)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _slots.erase(
            std::remove_if(_slots.begin(), _slots.end(),
                           [id](const Entry& e) { return e.id == id; }),
            _slots.end());
    }

    // Fire all live subscribers.
    void operator()(Args... args) const
    {
        std::vector<Entry> snapshot;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            snapshot = _slots;
        }

        for (const Entry& e : snapshot)
        {
            // If this was a weak_ptr connection and the owner has gone away,
            // skip it — the next fire will prune it.
            if (e.weak.has_value() && e.weak->expired())
                continue;
            e.slot(std::forward<Args>(args)...);
        }

        // Prune dead weak_ptr connections.
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _slots.erase(
                std::remove_if(_slots.begin(), _slots.end(),
                               [](const Entry& e) {
                                   return e.weak.has_value() && e.weak->expired();
                               }),
                _slots.end());
        }
    }

    // Returns true if at least one subscriber is connected.
    explicit operator bool() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return !_slots.empty();
    }

private:
    struct Entry
    {
        int                                  id;
        std::optional<std::weak_ptr<void>>   weak;  // set for lifetime-safe connections
        Slot                                 slot;
    };

    mutable std::mutex   _mutex;
    mutable std::vector<Entry> _slots;
    int                  _nextID{0};
};

} // namespace qtac
