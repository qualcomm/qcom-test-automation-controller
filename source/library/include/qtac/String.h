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

#ifndef QTAC_STRING_H
#define QTAC_STRING_H

#include <qtac/ByteArray.h>
#include <string>
#include <vector>
#include <cstdint>

#ifdef _WIN32
#include <string> // for std::wstring
#endif

namespace qtac {

class StringList;

class String
{
public:
	String();
	String(const char* str);
	String(const char* str, int size);
	String(const std::string& str);
	String(std::string&& str);
	String(const ByteArray& ba);
	String(char ch);

#ifdef _WIN32
	static String fromStdWString(const std::wstring& wstr);
	std::wstring toStdWString() const;
#endif

	// --- Size ---
	int size() const;
	int length() const;
	bool isEmpty() const;
	bool isNull() const;
	void clear();

	// --- Data access ---
	const char* constData() const;
	const char* data() const;
	char* data();
	std::string toStdString() const;

	// --- Conversion to ByteArray ---
	ByteArray toLatin1() const;
	ByteArray toUtf8() const;
	ByteArray toLocal8Bit() const;

	// --- Numeric conversions ---
	int toInt(bool* ok = nullptr, int base = 10) const;
	unsigned int toUInt(bool* ok = nullptr, int base = 10) const;
	long long toLongLong(bool* ok = nullptr, int base = 10) const;
	unsigned long long toULongLong(bool* ok = nullptr, int base = 10) const;
	double toDouble(bool* ok = nullptr) const;
	float toFloat(bool* ok = nullptr) const;

	// --- Case conversion ---
	String toUpper() const;
	String toLower() const;

	// --- Trimming ---
	String trimmed() const;
	String simplified() const;

	// --- Search ---
	bool contains(const String& str, bool caseSensitive = true) const;
	bool contains(char ch, bool caseSensitive = true) const;
	int indexOf(const String& str, int from = 0) const;
	int indexOf(char ch, int from = 0) const;
	int lastIndexOf(const String& str, int from = -1) const;
	int lastIndexOf(char ch, int from = -1) const;
	bool startsWith(const String& str, bool caseSensitive = true) const;
	bool startsWith(char ch, bool caseSensitive = true) const;
	bool endsWith(const String& str, bool caseSensitive = true) const;
	bool endsWith(char ch, bool caseSensitive = true) const;

	// --- Substrings ---
	String mid(int pos, int len = -1) const;
	String left(int len) const;
	String right(int len) const;

	// --- Split ---
	StringList split(const String& separator) const;
	StringList split(char separator) const;

	// --- Replace ---
	String& replace(const String& before, const String& after);
	String& replace(char before, char after);

	// --- Arg (Qt-style %1..%9 placeholder substitution) ---
	String arg(const String& a) const;
	String arg(int a) const;
	String arg(unsigned int a) const;
	String arg(long long a) const;
	String arg(unsigned long long a) const;
	String arg(double a, int fieldWidth = 0, char format = 'g', int precision = -1) const;
	String arg(const String& a1, const String& a2) const;
	String arg(const String& a1, const String& a2, const String& a3) const;

	// --- Compare ---
	int compare(const String& other, bool caseSensitive = true) const;
	static int compare(const String& s1, const String& s2, bool caseSensitive = true);

	// --- Static constructors ---
	static String number(int value, int base = 10);
	static String number(unsigned int value, int base = 10);
	static String number(long long value, int base = 10);
	static String number(unsigned long long value, int base = 10);
	static String number(double value, char format = 'g', int precision = 6);

	// --- Append ---
	String& append(const String& str);
	String& append(char ch);
	String& prepend(const String& str);

	// --- Operators ---
	String& operator+=(const String& other);
	String& operator+=(const char* str);
	String& operator+=(char ch);

	friend String operator+(const String& lhs, const String& rhs);
	friend String operator+(const String& lhs, const char* rhs);
	friend String operator+(const char* lhs, const String& rhs);

	// --- Comparison operators ---
	friend bool operator==(const String& lhs, const String& rhs);
	friend bool operator!=(const String& lhs, const String& rhs);
	friend bool operator<(const String& lhs, const String& rhs);
	friend bool operator>(const String& lhs, const String& rhs);
	friend bool operator<=(const String& lhs, const String& rhs);
	friend bool operator>=(const String& lhs, const String& rhs);
	friend bool operator==(const String& lhs, const char* rhs);
	friend bool operator==(const char* lhs, const String& rhs);
	friend bool operator!=(const String& lhs, const char* rhs);
	friend bool operator!=(const char* lhs, const String& rhs);

	// --- Iterators ---
	using iterator = std::string::iterator;
	using const_iterator = std::string::const_iterator;

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;

	// --- Hashing support ---
	struct Hash {
		size_t operator()(const String& s) const;
	};

private:
	std::string _data;
	bool _isNull = true;

	String argHelper(const String& a, int placeholder) const;
};

// QStringLiteral equivalent — just constructs a String
#define QStringLiteral(str) qtac::String(str)

} // namespace qtac

#endif // QTAC_STRING_H
