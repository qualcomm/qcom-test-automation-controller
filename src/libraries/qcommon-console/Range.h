#ifndef RANGE_H
#define RANGE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

template<class T> class basic_range
{
public:
	basic_range(T begin, T end) :
		_end(end),
		_iter(begin)
	{}

	basic_range(T end) :
		_end(end),
		_iter(0)
	{}

	// Iterable functions
	const basic_range& begin() const { return *this; }
	const basic_range& end() const { return *this; }

	// Iterator functions
	bool operator!=(const basic_range&) const { return _iter < _end; }
	void operator++() { ++_iter; }
	T operator*() const { return _iter; }

private:
	T _end;
	T _iter;
};

typedef basic_range<int> range;

template <typename T> class keyValueRange
{
public:
   keyValueRange(T &data) :
	   _data{data}
   {
   }

   auto begin() { return _data.keyValueBegin(); }
   auto end() { return _data.keyValueEnd(); }

private:
   T&							_data;
};

#endif // RANGE_H
