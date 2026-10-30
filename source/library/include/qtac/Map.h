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

#ifndef QTAC_MAP_H
#define QTAC_MAP_H

#include <map>
#include <vector>

namespace qtac {

template<typename Key, typename Value>
class Map
{
public:
	using iterator = typename std::map<Key, Value>::iterator;
	using const_iterator = typename std::map<Key, Value>::const_iterator;

	Map() = default;
	Map(std::initializer_list<std::pair<const Key, Value>> init) : _data(init) {}

	// --- Size ---
	int size() const { return static_cast<int>(_data.size()); }
	int count() const { return size(); }
	bool isEmpty() const { return _data.empty(); }
	void clear() { _data.clear(); }

	// --- Access ---
	Value value(const Key& key, const Value& defaultValue = Value()) const
	{
		auto it = _data.find(key);
		if (it == _data.end()) return defaultValue;
		return it->second;
	}

	Value& operator[](const Key& key) { return _data[key]; }
	const Value operator[](const Key& key) const
	{
		auto it = _data.find(key);
		if (it == _data.end()) return Value();
		return it->second;
	}

	// --- Modification ---
	void insert(const Key& key, const Value& value) { _data[key] = value; }

	int remove(const Key& key)
	{
		return static_cast<int>(_data.erase(key));
	}

	// --- Search ---
	bool contains(const Key& key) const
	{
		return _data.find(key) != _data.end();
	}

	iterator find(const Key& key) { return _data.find(key); }
	const_iterator find(const Key& key) const { return _data.find(key); }

	// --- Keys / Values ---
	std::vector<Key> keys() const
	{
		std::vector<Key> result;
		result.reserve(_data.size());
		for (const auto& pair : _data)
			result.push_back(pair.first);
		return result;
	}

	std::vector<Value> values() const
	{
		std::vector<Value> result;
		result.reserve(_data.size());
		for (const auto& pair : _data)
			result.push_back(pair.second);
		return result;
	}

	Key key(const Value& value, const Key& defaultKey = Key()) const
	{
		for (const auto& pair : _data) {
			if (pair.second == value) return pair.first;
		}
		return defaultKey;
	}

	// --- Iterators ---
	iterator begin() { return _data.begin(); }
	iterator end() { return _data.end(); }
	const_iterator begin() const { return _data.begin(); }
	const_iterator end() const { return _data.end(); }
	const_iterator cbegin() const { return _data.cbegin(); }
	const_iterator cend() const { return _data.cend(); }

	// --- Comparison ---
	friend bool operator==(const Map& lhs, const Map& rhs) { return lhs._data == rhs._data; }
	friend bool operator!=(const Map& lhs, const Map& rhs) { return lhs._data != rhs._data; }

private:
	std::map<Key, Value> _data;
};

} // namespace qtac

#endif // QTAC_MAP_H
