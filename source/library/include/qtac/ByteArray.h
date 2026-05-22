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

#ifndef QTAC_BYTEARRAY_H
#define QTAC_BYTEARRAY_H

#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <sstream>
#include <stdexcept>

namespace qtac {

class ByteArray
{
public:
	ByteArray()
		: _isNull(true)
	{
	}

	ByteArray(const char* data)
		: _data(data ? data : ""), _isNull(data == nullptr)
	{
	}

	ByteArray(const char* data, int size)
		: _data(data, static_cast<size_t>(size)), _isNull(false)
	{
	}

	ByteArray(int size, char ch)
		: _data(static_cast<size_t>(size), ch), _isNull(false)
	{
	}

	ByteArray(const std::string& str)
		: _data(str), _isNull(false)
	{
	}

	ByteArray(std::string&& str)
		: _data(std::move(str)), _isNull(false)
	{
	}

	// --- Size and capacity ---

	int size() const { return static_cast<int>(_data.size()); }
	int length() const { return size(); }
	bool isEmpty() const { return _data.empty(); }
	bool isNull() const { return _isNull; }
	void clear() { _data.clear(); _isNull = false; }
	void resize(int size) { _data.resize(static_cast<size_t>(size)); _isNull = false; }
	void reserve(int size) { _data.reserve(static_cast<size_t>(size)); }

	// --- Data access ---

	char* data() { return _data.data(); }
	const char* data() const { return _data.data(); }
	const char* constData() const { return _data.c_str(); }

	char at(int index) const { return _data.at(static_cast<size_t>(index)); }
	char& operator[](int index) { return _data[static_cast<size_t>(index)]; }
	char operator[](int index) const { return _data[static_cast<size_t>(index)]; }

	// --- Iterators ---

	using iterator = std::string::iterator;
	using const_iterator = std::string::const_iterator;

	iterator begin() { return _data.begin(); }
	iterator end() { return _data.end(); }
	const_iterator begin() const { return _data.begin(); }
	const_iterator end() const { return _data.end(); }
	const_iterator cbegin() const { return _data.cbegin(); }
	const_iterator cend() const { return _data.cend(); }

	// --- Append / Prepend ---

	ByteArray& append(const ByteArray& other)
	{
		_data.append(other._data);
		_isNull = false;
		return *this;
	}

	ByteArray& append(const char* data)
	{
		if (data)
			_data.append(data);
		_isNull = false;
		return *this;
	}

	ByteArray& append(const char* data, int size)
	{
		_data.append(data, static_cast<size_t>(size));
		_isNull = false;
		return *this;
	}

	ByteArray& append(char ch)
	{
		_data.push_back(ch);
		_isNull = false;
		return *this;
	}

	ByteArray& prepend(const ByteArray& other)
	{
		_data.insert(0, other._data);
		_isNull = false;
		return *this;
	}

	ByteArray& prepend(const char* data)
	{
		if (data)
			_data.insert(0, data);
		_isNull = false;
		return *this;
	}

	ByteArray& prepend(char ch)
	{
		_data.insert(_data.begin(), ch);
		_isNull = false;
		return *this;
	}

	// --- Insert / Remove ---

	ByteArray& insert(int pos, const ByteArray& other)
	{
		_data.insert(static_cast<size_t>(pos), other._data);
		_isNull = false;
		return *this;
	}

	ByteArray& insert(int pos, const char* data)
	{
		if (data)
			_data.insert(static_cast<size_t>(pos), data);
		_isNull = false;
		return *this;
	}

	ByteArray& insert(int pos, const char* data, int len)
	{
		_data.insert(static_cast<size_t>(pos), data, static_cast<size_t>(len));
		_isNull = false;
		return *this;
	}

	ByteArray& insert(int pos, char ch)
	{
		_data.insert(_data.begin() + pos, ch);
		_isNull = false;
		return *this;
	}

	ByteArray& insert(int pos, int count, char ch)
	{
		_data.insert(static_cast<size_t>(pos), static_cast<size_t>(count), ch);
		_isNull = false;
		return *this;
	}

	ByteArray& remove(int pos, int len)
	{
		_data.erase(static_cast<size_t>(pos), static_cast<size_t>(len));
		return *this;
	}

	void chop(int n)
	{
		if (n >= size())
			_data.clear();
		else if (n > 0)
			_data.erase(_data.size() - static_cast<size_t>(n));
	}

	ByteArray chopped(int len) const
	{
		if (len >= size()) return ByteArray("");
		return ByteArray(_data.substr(0, _data.size() - static_cast<size_t>(len)));
	}

	void truncate(int pos)
	{
		if (pos < size())
			_data.resize(static_cast<size_t>(pos));
	}

	// --- Search ---

	bool contains(const ByteArray& other) const
	{
		return _data.find(other._data) != std::string::npos;
	}

	bool contains(const char* data) const
	{
		return data && _data.find(data) != std::string::npos;
	}

	bool contains(char ch) const
	{
		return _data.find(ch) != std::string::npos;
	}

	int indexOf(const ByteArray& other, int from = 0) const
	{
		auto pos = _data.find(other._data, static_cast<size_t>(from));
		return pos == std::string::npos ? -1 : static_cast<int>(pos);
	}

	int indexOf(char ch, int from = 0) const
	{
		auto pos = _data.find(ch, static_cast<size_t>(from));
		return pos == std::string::npos ? -1 : static_cast<int>(pos);
	}

	int lastIndexOf(const ByteArray& other, int from = -1) const
	{
		size_t searchFrom = (from < 0) ? std::string::npos : static_cast<size_t>(from);
		auto pos = _data.rfind(other._data, searchFrom);
		return pos == std::string::npos ? -1 : static_cast<int>(pos);
	}

	int lastIndexOf(char ch, int from = -1) const
	{
		size_t searchFrom = (from < 0) ? std::string::npos : static_cast<size_t>(from);
		auto pos = _data.rfind(ch, searchFrom);
		return pos == std::string::npos ? -1 : static_cast<int>(pos);
	}

	bool startsWith(const ByteArray& other) const
	{
		return _data.size() >= other._data.size() &&
		       _data.compare(0, other._data.size(), other._data) == 0;
	}

	bool startsWith(char ch) const
	{
		return !_data.empty() && _data.front() == ch;
	}

	bool endsWith(const ByteArray& other) const
	{
		return _data.size() >= other._data.size() &&
		       _data.compare(_data.size() - other._data.size(), other._data.size(), other._data) == 0;
	}

	bool endsWith(char ch) const
	{
		return !_data.empty() && _data.back() == ch;
	}

	// --- Count ---

	int count(const ByteArray& other) const
	{
		if (other._data.empty()) return 0;
		int n = 0;
		size_t pos = 0;
		while ((pos = _data.find(other._data, pos)) != std::string::npos) {
			++n;
			pos += other._data.size();
		}
		return n;
	}

	int count(const char* data) const
	{
		if (!data || !*data) return 0;
		int n = 0;
		size_t len = std::strlen(data);
		size_t pos = 0;
		while ((pos = _data.find(data, pos)) != std::string::npos) {
			++n;
			pos += len;
		}
		return n;
	}

	int count(char ch) const
	{
		return static_cast<int>(std::count(_data.begin(), _data.end(), ch));
	}

	// --- Substrings ---

	ByteArray left(int len) const
	{
		return ByteArray(_data.substr(0, static_cast<size_t>(len)));
	}

	ByteArray right(int len) const
	{
		if (len >= size()) return *this;
		return ByteArray(_data.substr(_data.size() - static_cast<size_t>(len)));
	}

	ByteArray mid(int pos, int len = -1) const
	{
		if (pos >= size()) return ByteArray();
		if (len < 0)
			return ByteArray(_data.substr(static_cast<size_t>(pos)));
		return ByteArray(_data.substr(static_cast<size_t>(pos), static_cast<size_t>(len)));
	}

	// --- Case conversion ---

	ByteArray toLower() const
	{
		std::string result = _data;
		std::transform(result.begin(), result.end(), result.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return ByteArray(std::move(result));
	}

	ByteArray toUpper() const
	{
		std::string result = _data;
		std::transform(result.begin(), result.end(), result.begin(),
			[](unsigned char c) { return static_cast<char>(std::toupper(c)); });
		return ByteArray(std::move(result));
	}

	// --- Trimming ---

	ByteArray trimmed() const
	{
		if (_data.empty()) return ByteArray();
		auto start = _data.find_first_not_of(" \t\n\r\f\v");
		if (start == std::string::npos) return ByteArray();
		auto end = _data.find_last_not_of(" \t\n\r\f\v");
		return ByteArray(_data.substr(start, end - start + 1));
	}

	// --- Numeric conversions ---

	int toInt(bool* ok = nullptr, int base = 10) const
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

	double toDouble(bool* ok = nullptr) const
	{
		try {
			size_t pos = 0;
			double result = std::stod(_data, &pos);
			if (ok) *ok = (pos == _data.size());
			return result;
		} catch (...) {
			if (ok) *ok = false;
			return 0.0;
		}
	}

	long toLong(bool* ok = nullptr, int base = 10) const
	{
		try {
			size_t pos = 0;
			long result = std::stol(_data, &pos, base);
			if (ok) *ok = (pos == _data.size());
			return result;
		} catch (...) {
			if (ok) *ok = false;
			return 0;
		}
	}

	unsigned int toUInt(bool* ok = nullptr, int base = 10) const
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

	long long toLongLong(bool* ok = nullptr, int base = 10) const
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

	unsigned long long toULongLong(bool* ok = nullptr, int base = 10) const
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

	// --- Static number-to-ByteArray ---

	static ByteArray number(int value, int base = 10)
	{
		if (base == 10) {
			return ByteArray(std::to_string(value));
		}
		std::ostringstream oss;
		if (base == 16)
			oss << std::hex << value;
		else if (base == 8)
			oss << std::oct << value;
		else
			oss << value;
		return ByteArray(oss.str());
	}

	static ByteArray number(double value, char format = 'g', int precision = 6)
	{
		std::ostringstream oss;
		if (format == 'f')
			oss << std::fixed;
		else if (format == 'e')
			oss << std::scientific;
		oss.precision(precision);
		oss << value;
		return ByteArray(oss.str());
	}

	// --- Replace ---

	ByteArray& replace(const ByteArray& before, const ByteArray& after)
	{
		size_t pos = 0;
		while ((pos = _data.find(before._data, pos)) != std::string::npos) {
			_data.replace(pos, before._data.size(), after._data);
			pos += after._data.size();
		}
		return *this;
	}

	ByteArray& replace(char before, char after)
	{
		std::replace(_data.begin(), _data.end(), before, after);
		return *this;
	}

	// --- Split ---

	std::vector<ByteArray> split(char separator) const
	{
		std::vector<ByteArray> result;
		size_t start = 0;
		size_t pos = _data.find(separator);
		while (pos != std::string::npos) {
			result.emplace_back(_data.substr(start, pos - start));
			start = pos + 1;
			pos = _data.find(separator, start);
		}
		result.emplace_back(_data.substr(start));
		return result;
	}

	// --- Operators ---

	ByteArray& operator+=(const ByteArray& other)
	{
		_data += other._data;
		return *this;
	}

	ByteArray& operator+=(const char* data)
	{
		if (data) _data += data;
		return *this;
	}

	ByteArray& operator+=(char ch)
	{
		_data += ch;
		return *this;
	}

	friend ByteArray operator+(const ByteArray& lhs, const ByteArray& rhs)
	{
		return ByteArray(lhs._data + rhs._data);
	}

	friend ByteArray operator+(const ByteArray& lhs, const char* rhs)
	{
		return ByteArray(lhs._data + (rhs ? rhs : ""));
	}

	friend ByteArray operator+(const char* lhs, const ByteArray& rhs)
	{
		return ByteArray((lhs ? lhs : "") + rhs._data);
	}

	// --- Comparison ---

	friend bool operator==(const ByteArray& lhs, const ByteArray& rhs) { return lhs._data == rhs._data; }
	friend bool operator!=(const ByteArray& lhs, const ByteArray& rhs) { return lhs._data != rhs._data; }
	friend bool operator<(const ByteArray& lhs, const ByteArray& rhs) { return lhs._data < rhs._data; }
	friend bool operator>(const ByteArray& lhs, const ByteArray& rhs) { return lhs._data > rhs._data; }
	friend bool operator<=(const ByteArray& lhs, const ByteArray& rhs) { return lhs._data <= rhs._data; }
	friend bool operator>=(const ByteArray& lhs, const ByteArray& rhs) { return lhs._data >= rhs._data; }

	friend bool operator==(const ByteArray& lhs, const char* rhs) { return lhs._data == (rhs ? rhs : ""); }
	friend bool operator==(const char* lhs, const ByteArray& rhs) { return (lhs ? lhs : "") == rhs._data; }
	friend bool operator!=(const ByteArray& lhs, const char* rhs) { return !(lhs == rhs); }
	friend bool operator!=(const char* lhs, const ByteArray& rhs) { return !(lhs == rhs); }

	// --- Compare ---

	int compare(const ByteArray& other, bool caseSensitive = true) const
	{
		if (caseSensitive) {
			return _data.compare(other._data);
		}
		auto lhs = toLower();
		auto rhs = other.toLower();
		return lhs._data.compare(rhs._data);
	}

	int compare(const char* other, bool caseSensitive = true) const
	{
		return compare(ByteArray(other), caseSensitive);
	}

	// --- Hex / Base64 encoding ---

	ByteArray toHex(char separator = '\0') const
	{
		static const char hexChars[] = "0123456789abcdef";
		std::string result;
		bool useSep = (separator != '\0');
		result.reserve(_data.size() * (useSep ? 3 : 2));
		for (size_t i = 0; i < _data.size(); ++i) {
			if (useSep && i > 0)
				result.push_back(separator);
			unsigned char c = static_cast<unsigned char>(_data[i]);
			result.push_back(hexChars[c >> 4]);
			result.push_back(hexChars[c & 0x0F]);
		}
		return ByteArray(std::move(result));
	}

	static ByteArray fromHex(const ByteArray& hex)
	{
		std::string result;
		result.reserve(hex._data.size() / 2);
		for (size_t i = 0; i < hex._data.size(); ++i) {
			char c = hex._data[i];
			int nibble = -1;
			if (c >= '0' && c <= '9') nibble = c - '0';
			else if (c >= 'a' && c <= 'f') nibble = c - 'a' + 10;
			else if (c >= 'A' && c <= 'F') nibble = c - 'A' + 10;
			else continue; // skip non-hex chars (separators, whitespace)

			// Look for next hex nibble
			++i;
			while (i < hex._data.size()) {
				char c2 = hex._data[i];
				int nibble2 = -1;
				if (c2 >= '0' && c2 <= '9') nibble2 = c2 - '0';
				else if (c2 >= 'a' && c2 <= 'f') nibble2 = c2 - 'a' + 10;
				else if (c2 >= 'A' && c2 <= 'F') nibble2 = c2 - 'A' + 10;
				else { ++i; continue; }
				result.push_back(static_cast<char>((nibble << 4) | nibble2));
				break;
			}
		}
		return ByteArray(std::move(result));
	}

	ByteArray toBase64() const
	{
		static const char table[] =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		std::string result;
		size_t len = _data.size();
		result.reserve(((len + 2) / 3) * 4);
		for (size_t i = 0; i < len; i += 3) {
			unsigned int b0 = static_cast<unsigned char>(_data[i]);
			unsigned int b1 = (i + 1 < len) ? static_cast<unsigned char>(_data[i + 1]) : 0u;
			unsigned int b2 = (i + 2 < len) ? static_cast<unsigned char>(_data[i + 2]) : 0u;
			unsigned int triple = (b0 << 16) | (b1 << 8) | b2;
			result.push_back(table[(triple >> 18) & 0x3F]);
			result.push_back(table[(triple >> 12) & 0x3F]);
			result.push_back((i + 1 < len) ? table[(triple >> 6) & 0x3F] : '=');
			result.push_back((i + 2 < len) ? table[triple & 0x3F] : '=');
		}
		return ByteArray(std::move(result));
	}

	static ByteArray fromBase64(const ByteArray& base64)
	{
		auto decodeChar = [](char c) -> int {
			if (c >= 'A' && c <= 'Z') return c - 'A';
			if (c >= 'a' && c <= 'z') return c - 'a' + 26;
			if (c >= '0' && c <= '9') return c - '0' + 52;
			if (c == '+') return 62;
			if (c == '/') return 63;
			return -1;
		};
		std::string result;
		result.reserve((base64._data.size() / 4) * 3);
		int buf = 0, bits = 0;
		for (char c : base64._data) {
			int val = decodeChar(c);
			if (val < 0) continue;
			buf = (buf << 6) | val;
			bits += 6;
			if (bits >= 8) {
				bits -= 8;
				result.push_back(static_cast<char>((buf >> bits) & 0xFF));
			}
		}
		return ByteArray(std::move(result));
	}

	// --- Conversion to std::string ---

	std::string toStdString() const { return _data; }

	// --- Hashing support (for use in unordered containers) ---

	struct Hash {
		size_t operator()(const ByteArray& ba) const
		{
			return std::hash<std::string>{}(ba._data);
		}
	};

private:
	std::string _data;
	bool _isNull = true;
};

} // namespace qtac

#endif // QTAC_BYTEARRAY_H
