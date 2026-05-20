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

#include <qtac/StringList.h>
#include <algorithm>
#include <stdexcept>

namespace qtac {

// --- Constructors ---

StringList::StringList() {}
StringList::StringList(std::initializer_list<String> init) : _data(init) {}
StringList::StringList(const std::vector<String>& vec) : _data(vec) {}
StringList::StringList(std::vector<String>&& vec) : _data(std::move(vec)) {}

// --- Size ---

int StringList::size() const { return static_cast<int>(_data.size()); }
int StringList::count() const { return size(); }
bool StringList::isEmpty() const { return _data.empty(); }

// --- Element access ---

const String& StringList::at(int index) const { return _data.at(static_cast<size_t>(index)); }
String& StringList::operator[](int index) { return _data[static_cast<size_t>(index)]; }
const String& StringList::operator[](int index) const { return _data[static_cast<size_t>(index)]; }
const String& StringList::first() const { return _data.front(); }
const String& StringList::last() const { return _data.back(); }

// --- Modification ---

void StringList::append(const String& str) { _data.push_back(str); }
void StringList::append(const StringList& other)
{
	_data.insert(_data.end(), other._data.begin(), other._data.end());
}
void StringList::prepend(const String& str) { _data.insert(_data.begin(), str); }
void StringList::insert(int index, const String& str)
{
	_data.insert(_data.begin() + index, str);
}
void StringList::removeAt(int index) { _data.erase(_data.begin() + index); }
void StringList::clear() { _data.clear(); }

// --- Search ---

bool StringList::contains(const String& str) const
{
	return std::find(_data.begin(), _data.end(), str) != _data.end();
}

int StringList::indexOf(const String& str) const
{
	auto it = std::find(_data.begin(), _data.end(), str);
	if (it == _data.end()) return -1;
	return static_cast<int>(std::distance(_data.begin(), it));
}

// --- Operations ---

String StringList::join(const String& separator) const
{
	if (_data.empty()) return String("");
	std::string result;
	for (size_t i = 0; i < _data.size(); ++i) {
		if (i > 0) result += separator.toStdString();
		result += _data[i].toStdString();
	}
	return String(std::move(result));
}

String StringList::join(char separator) const
{
	return join(String(separator));
}

StringList StringList::filter(const String& str) const
{
	StringList result;
	for (const auto& s : _data) {
		if (s.contains(str))
			result.append(s);
	}
	return result;
}

// --- Operators ---

StringList& StringList::operator<<(const String& str) { _data.push_back(str); return *this; }
StringList& StringList::operator+=(const String& str) { _data.push_back(str); return *this; }
StringList& StringList::operator+=(const StringList& other)
{
	_data.insert(_data.end(), other._data.begin(), other._data.end());
	return *this;
}

// --- Iterators ---

StringList::iterator StringList::begin() { return _data.begin(); }
StringList::iterator StringList::end() { return _data.end(); }
StringList::const_iterator StringList::begin() const { return _data.begin(); }
StringList::const_iterator StringList::end() const { return _data.end(); }
StringList::const_iterator StringList::cbegin() const { return _data.cbegin(); }
StringList::const_iterator StringList::cend() const { return _data.cend(); }

// --- Conversion ---

std::vector<String>& StringList::toVector() { return _data; }
const std::vector<String>& StringList::toVector() const { return _data; }

// --- String::split implementations (defined here to avoid circular dependency) ---

StringList String::split(const String& separator) const
{
	StringList result;
	if (separator.isEmpty()) {
		result.append(*this);
		return result;
	}
	std::string sep = separator.toStdString();
	size_t start = 0;
	size_t pos = _data.find(sep);
	while (pos != std::string::npos) {
		result.append(String(_data.substr(start, pos - start)));
		start = pos + sep.size();
		pos = _data.find(sep, start);
	}
	result.append(String(_data.substr(start)));
	return result;
}

StringList String::split(char separator) const
{
	StringList result;
	size_t start = 0;
	size_t pos = _data.find(separator);
	while (pos != std::string::npos) {
		result.append(String(_data.substr(start, pos - start)));
		start = pos + 1;
		pos = _data.find(separator, start);
	}
	result.append(String(_data.substr(start)));
	return result;
}

} // namespace qtac
