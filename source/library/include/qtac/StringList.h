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

#ifndef QTAC_STRINGLIST_H
#define QTAC_STRINGLIST_H

#include <qtac/String.h>
#include <vector>
#include <initializer_list>

namespace qtac {

class StringList
{
public:
	StringList();
	StringList(std::initializer_list<String> init);
	StringList(const std::vector<String>& vec);
	StringList(std::vector<String>&& vec);

	// --- Size ---
	int size() const;
	int count() const;
	bool isEmpty() const;

	// --- Element access ---
	const String& at(int index) const;
	String& operator[](int index);
	const String& operator[](int index) const;
	const String& first() const;
	const String& last() const;

	// --- Modification ---
	void append(const String& str);
	void append(const StringList& other);
	void prepend(const String& str);
	void insert(int index, const String& str);
	void removeAt(int index);
	void clear();

	// --- Search ---
	bool contains(const String& str) const;
	int indexOf(const String& str) const;

	// --- Operations ---
	String join(const String& separator) const;
	String join(char separator) const;
	StringList filter(const String& str) const;

	// --- Operators ---
	StringList& operator<<(const String& str);
	StringList& operator+=(const String& str);
	StringList& operator+=(const StringList& other);

	// --- Iterators ---
	using iterator = std::vector<String>::iterator;
	using const_iterator = std::vector<String>::const_iterator;

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;

	// --- Conversion ---
	std::vector<String>& toVector();
	const std::vector<String>& toVector() const;

private:
	std::vector<String> _data;
};

} // namespace qtac

#endif // QTAC_STRINGLIST_H
