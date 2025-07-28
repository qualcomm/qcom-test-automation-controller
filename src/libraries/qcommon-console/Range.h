#ifndef RANGE_H
#define RANGE_H
// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2013-2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
