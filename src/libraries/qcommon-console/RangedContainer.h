#ifndef RANGEDCONTAINER_H
#define RANGEDCONTAINER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

template <typename T>
class RangedContainer
{
public:
	RangedContainer(T &data) :
		_container{data}
	{
	}

	auto begin() { return _container.keyValueBegin(); }

	auto end() { return _container.keyValueEnd(); }

private:
	T							_container;
};

#endif // RANGEDCONTAINER_H
