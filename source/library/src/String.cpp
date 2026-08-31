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

#include <qtac/String.h>
#include <qtac/StringList.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif

namespace qtac {

// --- Constructors ---

String::String() : _isNull(true) {}

String::String(const char* str)
	: _data(str ? str : ""), _isNull(str == nullptr)
{
}

String::String(const char* str, int size)
	: _data(str, static_cast<size_t>(size)), _isNull(false)
{
}

String::String(const std::string& str)
	: _data(str), _isNull(false)
{
}

String::String(std::string&& str)
	: _data(std::move(str)), _isNull(false)
{
}

String::String(const ByteArray& ba)
	: _data(ba.constData(), static_cast<size_t>(ba.size())), _isNull(ba.isNull())
{
}

String::String(char ch)
	: _data(1, ch), _isNull(false)
{
}

#ifdef _WIN32
String String::fromStdWString(const std::wstring& wstr)
{
	if (wstr.empty()) return String("");
	int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
		static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
	std::string result(static_cast<size_t>(sizeNeeded), '\0');
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
		static_cast<int>(wstr.size()), &result[0], sizeNeeded, nullptr, nullptr);
	return String(std::move(result));
}

std::wstring String::toStdWString() const
{
	if (_data.empty()) return std::wstring();
	int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, _data.c_str(),
		static_cast<int>(_data.size()), nullptr, 0);
	std::wstring result(static_cast<size_t>(sizeNeeded), L'\0');
	MultiByteToWideChar(CP_UTF8, 0, _data.c_str(),
		static_cast<int>(_data.size()), &result[0], sizeNeeded);
	return result;
}
#endif

// --- Size ---

int String::size() const { return static_cast<int>(_data.size()); }
int String::length() const { return size(); }
bool String::isEmpty() const { return _data.empty(); }
bool String::isNull() const { return _isNull; }
void String::clear() { _data.clear(); _isNull = false; }

// --- Data access ---

const char* String::constData() const { return _data.c_str(); }
const char* String::data() const { return _data.c_str(); }
char* String::data() { return _data.empty() ? nullptr : &_data[0]; }
std::string String::toStdString() const { return _data; }

// --- Conversion to ByteArray ---

ByteArray String::toLatin1() const { return ByteArray(_data); }

// --- Numeric conversions ---

int String::toInt(bool* ok, int base) const
{
	try {
		size_t pos = 0;
		int result = std::stoi(_data, &pos, base);
		if (ok) *ok = (pos == _data.size());
		return result;
	} catch (...) {
		if (ok) *ok = false;
		return 0;
	}
}

unsigned int String::toUInt(bool* ok, int base) const
{
	try {
		size_t pos = 0;
		unsigned long result = std::stoul(_data, &pos, base);
		if (ok) *ok = (pos == _data.size() && result <= UINT32_MAX);
		return static_cast<unsigned int>(result);
	} catch (...) {
		if (ok) *ok = false;
		return 0;
	}
}

long long String::toLongLong(bool* ok, int base) const
{
	try {
		size_t pos = 0;
		long long result = std::stoll(_data, &pos, base);
		if (ok) *ok = (pos == _data.size());
		return result;
	} catch (...) {
		if (ok) *ok = false;
		return 0;
	}
}

unsigned long long String::toULongLong(bool* ok, int base) const
{
	try {
		size_t pos = 0;
		unsigned long long result = std::stoull(_data, &pos, base);
		if (ok) *ok = (pos == _data.size());
		return result;
	} catch (...) {
		if (ok) *ok = false;
		return 0;
	}
}

// --- Case conversion ---

String String::toUpper() const
{
	std::string result = _data;
	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char c) { return static_cast<char>(std::toupper(c)); });
	return String(std::move(result));
}

String String::toLower() const
{
	std::string result = _data;
	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return String(std::move(result));
}

// --- Trimming ---

String String::trimmed() const
{
	if (_data.empty()) return String("");
	auto start = _data.find_first_not_of(" \t\n\r\f\v");
	if (start == std::string::npos) return String("");
	auto end = _data.find_last_not_of(" \t\n\r\f\v");
	return String(_data.substr(start, end - start + 1));
}

// --- Search ---

bool String::contains(const String& str, bool caseSensitive) const
{
	if (caseSensitive)
		return _data.find(str._data) != std::string::npos;
	return toLower()._data.find(str.toLower()._data) != std::string::npos;
}

bool String::contains(char ch, bool caseSensitive) const
{
	if (caseSensitive)
		return _data.find(ch) != std::string::npos;
	char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
	return toLower()._data.find(lower) != std::string::npos;
}

int String::indexOf(const String& str, int from) const
{
	auto pos = _data.find(str._data, static_cast<size_t>(from));
	return pos == std::string::npos ? -1 : static_cast<int>(pos);
}

int String::indexOf(char ch, int from) const
{
	auto pos = _data.find(ch, static_cast<size_t>(from));
	return pos == std::string::npos ? -1 : static_cast<int>(pos);
}

bool String::startsWith(const String& str, bool caseSensitive) const
{
	if (str._data.size() > _data.size()) return false;
	if (caseSensitive)
		return _data.compare(0, str._data.size(), str._data) == 0;
	return toLower()._data.compare(0, str._data.size(), str.toLower()._data) == 0;
}

bool String::startsWith(char ch, bool caseSensitive) const
{
	if (_data.empty()) return false;
	if (caseSensitive)
		return _data.front() == ch;
	return std::tolower(static_cast<unsigned char>(_data.front())) ==
	       std::tolower(static_cast<unsigned char>(ch));
}

bool String::endsWith(const String& str, bool caseSensitive) const
{
	if (str._data.size() > _data.size()) return false;
	if (caseSensitive)
		return _data.compare(_data.size() - str._data.size(), str._data.size(), str._data) == 0;
	return toLower()._data.compare(_data.size() - str._data.size(), str._data.size(), str.toLower()._data) == 0;
}

bool String::endsWith(char ch, bool caseSensitive) const
{
	if (_data.empty()) return false;
	if (caseSensitive)
		return _data.back() == ch;
	return std::tolower(static_cast<unsigned char>(_data.back())) ==
	       std::tolower(static_cast<unsigned char>(ch));
}

// --- Substrings ---

String String::left(int len) const
{
	return String(_data.substr(0, static_cast<size_t>(len)));
}

String String::mid(int position, int len) const
{
	if (position < 0) position = 0;
	if (static_cast<size_t>(position) >= _data.size()) return String("");
	if (len < 0)
		return String(_data.substr(static_cast<size_t>(position)));
	return String(_data.substr(static_cast<size_t>(position), static_cast<size_t>(len)));
}

// --- Replace ---

String& String::replace(const String& before, const String& after)
{
	if (before._data.empty()) return *this;
	size_t pos = 0;
	while ((pos = _data.find(before._data, pos)) != std::string::npos) {
		_data.replace(pos, before._data.size(), after._data);
		pos += after._data.size();
	}
	return *this;
}

String& String::replace(char before, char after)
{
	std::replace(_data.begin(), _data.end(), before, after);
	return *this;
}

// --- Arg ---

String String::argHelper(const String& a, int placeholder) const
{
	std::string marker = "%" + std::to_string(placeholder);
	std::string result = _data;
	size_t pos = result.find(marker);
	if (pos != std::string::npos) {
		result.replace(pos, marker.size(), a._data);
	}
	return String(std::move(result));
}

String String::arg(const String& a) const
{
	// Find lowest placeholder %1..%9
	int lowest = 10;
	for (int i = 1; i <= 9; ++i) {
		std::string marker = "%" + std::to_string(i);
		if (_data.find(marker) != std::string::npos) {
			lowest = i;
			break;
		}
	}
	if (lowest > 9) return *this;
	// Replace all instances of lowest placeholder
	std::string marker = "%" + std::to_string(lowest);
	std::string result = _data;
	size_t pos = 0;
	while ((pos = result.find(marker, pos)) != std::string::npos) {
		result.replace(pos, marker.size(), a._data);
		pos += a._data.size();
	}
	return String(std::move(result));
}

String String::arg(int a) const { return arg(number(a)); }
String String::arg(unsigned int a) const { return arg(number(a)); }
String String::arg(long long a) const { return arg(number(a)); }
String String::arg(unsigned long long a) const { return arg(number(a)); }

String String::arg(double a, int /*fieldWidth*/, char format, int precision) const
{
	return arg(number(a, format, precision));
}

String String::arg(const String& a1, const String& a2) const
{
	return arg(a1).arg(a2);
}

String String::arg(const String& a1, const String& a2, const String& a3) const
{
	return arg(a1).arg(a2).arg(a3);
}

// --- Compare ---

int String::compare(const String& other, bool caseSensitive) const
{
	if (caseSensitive)
		return _data.compare(other._data);
	return toLower()._data.compare(other.toLower()._data);
}

int String::compare(const String& s1, const String& s2, bool caseSensitive)
{
	return s1.compare(s2, caseSensitive);
}

// --- Static number ---

String String::number(int value, int base)
{
	if (base == 10) return String(std::to_string(value));
	std::ostringstream oss;
	if (base == 16) oss << std::hex << value;
	else if (base == 8) oss << std::oct << value;
	else oss << value;
	return String(oss.str());
}

String String::number(unsigned int value, int base)
{
	if (base == 10) return String(std::to_string(value));
	std::ostringstream oss;
	if (base == 16) oss << std::hex << value;
	else if (base == 8) oss << std::oct << value;
	else oss << value;
	return String(oss.str());
}

String String::number(long long value, int base)
{
	if (base == 10) return String(std::to_string(value));
	std::ostringstream oss;
	if (base == 16) oss << std::hex << value;
	else if (base == 8) oss << std::oct << value;
	else oss << value;
	return String(oss.str());
}

String String::number(unsigned long long value, int base)
{
	if (base == 10) return String(std::to_string(value));
	std::ostringstream oss;
	if (base == 16) oss << std::hex << value;
	else if (base == 8) oss << std::oct << value;
	else oss << value;
	return String(oss.str());
}

String String::number(double value, char format, int precision)
{
	std::ostringstream oss;
	if (format == 'f')
		oss << std::fixed;
	else if (format == 'e')
		oss << std::scientific;
	oss << std::setprecision(precision) << value;
	return String(oss.str());
}

// --- Append ---

String& String::append(const String& str) { _data += str._data; _isNull = false; return *this; }
String& String::append(char ch) { _data += ch; _isNull = false; return *this; }

// --- Operators ---

String& String::operator+=(const String& other) { _data += other._data; _isNull = false; return *this; }
String& String::operator+=(const char* str) { if (str) _data += str; _isNull = false; return *this; }
String& String::operator+=(char ch) { _data += ch; _isNull = false; return *this; }

String operator+(const String& lhs, const String& rhs) { return String(lhs._data + rhs._data); }
String operator+(const String& lhs, const char* rhs) { return String(lhs._data + (rhs ? rhs : "")); }
String operator+(const char* lhs, const String& rhs) { return String((lhs ? lhs : "") + rhs._data); }

// --- Comparison ---

bool operator==(const String& lhs, const String& rhs) { return lhs._data == rhs._data; }
bool operator!=(const String& lhs, const String& rhs) { return lhs._data != rhs._data; }
bool operator<(const String& lhs, const String& rhs) { return lhs._data < rhs._data; }
bool operator>(const String& lhs, const String& rhs) { return lhs._data > rhs._data; }
bool operator<=(const String& lhs, const String& rhs) { return lhs._data <= rhs._data; }
bool operator>=(const String& lhs, const String& rhs) { return lhs._data >= rhs._data; }
bool operator==(const String& lhs, const char* rhs) { return lhs._data == (rhs ? rhs : ""); }
bool operator==(const char* lhs, const String& rhs) { return (lhs ? lhs : "") == rhs._data; }
bool operator!=(const String& lhs, const char* rhs) { return !(lhs == rhs); }
bool operator!=(const char* lhs, const String& rhs) { return !(lhs == rhs); }

// --- Iterators ---

String::iterator String::begin() { return _data.begin(); }
String::iterator String::end() { return _data.end(); }
String::const_iterator String::begin() const { return _data.begin(); }
String::const_iterator String::end() const { return _data.end(); }
String::const_iterator String::cbegin() const { return _data.cbegin(); }
String::const_iterator String::cend() const { return _data.cend(); }

// --- Hash ---

size_t String::Hash::operator()(const String& s) const
{
	return std::hash<std::string>{}(s._data);
}

} // namespace qtac
