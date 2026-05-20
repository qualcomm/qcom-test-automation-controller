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

#ifndef QTAC_LIST_H
#define QTAC_LIST_H

#include <vector>
#include <algorithm>
#include <initializer_list>
#include <stdexcept>

namespace qtac {

template<typename T>
class List
{
public:
	using iterator = typename std::vector<T>::iterator;
	using const_iterator = typename std::vector<T>::const_iterator;

	List() = default;
	List(std::initializer_list<T> init) : _data(init) {}
	List(const std::vector<T>& vec) : _data(vec) {}
	List(std::vector<T>&& vec) : _data(std::move(vec)) {}

	// --- Size ---
	int size() const { return static_cast<int>(_data.size()); }
	int count() const { return size(); }
	int length() const { return size(); }
	bool isEmpty() const { return _data.empty(); }
	void clear() { _data.clear(); }
	void reserve(int size) { _data.reserve(static_cast<size_t>(size)); }

	// --- Element access ---
	const T& at(int index) const { return _data.at(static_cast<size_t>(index)); }
	T& operator[](int index) { return _data[static_cast<size_t>(index)]; }
	const T& operator[](int index) const { return _data[static_cast<size_t>(index)]; }
	const T& first() const { return _data.front(); }
	T& first() { return _data.front(); }
	const T& last() const { return _data.back(); }
	T& last() { return _data.back(); }

	// --- Modification ---
	void append(const T& item) { _data.push_back(item); }
	void append(T&& item) { _data.push_back(std::move(item)); }
	void append(const List<T>& other)
	{
		_data.insert(_data.end(), other._data.begin(), other._data.end());
	}

	void prepend(const T& item) { _data.insert(_data.begin(), item); }

	void insert(int index, const T& item)
	{
		_data.insert(_data.begin() + index, item);
	}

	void removeAt(int index)
	{
		_data.erase(_data.begin() + index);
	}

	bool removeOne(const T& item)
	{
		auto it = std::find(_data.begin(), _data.end(), item);
		if (it == _data.end()) return false;
		_data.erase(it);
		return true;
	}

	int removeAll(const T& item)
	{
		auto it = std::remove(_data.begin(), _data.end(), item);
		int removed = static_cast<int>(std::distance(it, _data.end()));
		_data.erase(it, _data.end());
		return removed;
	}

	void replace(int index, const T& item)
	{
		_data[static_cast<size_t>(index)] = item;
	}

	T takeAt(int index)
	{
		T item = std::move(_data[static_cast<size_t>(index)]);
		_data.erase(_data.begin() + index);
		return item;
	}

	T takeFirst() { return takeAt(0); }
	T takeLast()
	{
		T item = std::move(_data.back());
		_data.pop_back();
		return item;
	}

	// --- Search ---
	bool contains(const T& item) const
	{
		return std::find(_data.begin(), _data.end(), item) != _data.end();
	}

	int indexOf(const T& item, int from = 0) const
	{
		auto it = std::find(_data.begin() + from, _data.end(), item);
		if (it == _data.end()) return -1;
		return static_cast<int>(std::distance(_data.begin(), it));
	}

	int lastIndexOf(const T& item, int from = -1) const
	{
		auto start = (from < 0) ? _data.rbegin() : _data.rbegin() + (size() - 1 - from);
		auto it = std::find(start, _data.rend(), item);
		if (it == _data.rend()) return -1;
		return static_cast<int>(std::distance(_data.begin(), it.base()) - 1);
	}

	// --- Operators ---
	List<T>& operator<<(const T& item) { _data.push_back(item); return *this; }
	List<T>& operator+=(const T& item) { _data.push_back(item); return *this; }
	List<T>& operator+=(const List<T>& other)
	{
		_data.insert(_data.end(), other._data.begin(), other._data.end());
		return *this;
	}

	friend List<T> operator+(const List<T>& lhs, const List<T>& rhs)
	{
		List<T> result = lhs;
		result += rhs;
		return result;
	}

	// --- Iterators ---
	iterator begin() { return _data.begin(); }
	iterator end() { return _data.end(); }
	const_iterator begin() const { return _data.begin(); }
	const_iterator end() const { return _data.end(); }
	const_iterator cbegin() const { return _data.cbegin(); }
	const_iterator cend() const { return _data.cend(); }

	// --- Conversion ---
	std::vector<T>& toVector() { return _data; }
	const std::vector<T>& toVector() const { return _data; }

	// --- Comparison ---
	friend bool operator==(const List& lhs, const List& rhs) { return lhs._data == rhs._data; }
	friend bool operator!=(const List& lhs, const List& rhs) { return lhs._data != rhs._data; }

private:
	std::vector<T> _data;
};

} // namespace qtac

#endif // QTAC_LIST_H
