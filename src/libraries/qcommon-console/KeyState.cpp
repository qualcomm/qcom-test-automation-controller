// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/


#include "KeyState.h"

#ifdef Q_OS_LINUX
	#include <fcntl.h>
	#include <unistd.h>
	#include <linux/input.h>
#endif

bool TestKeysState(QStack<char> &testChars)
{
	bool result(false);

	Q_UNUSED(testChars)

#ifdef Q_OS_LINUX
	quint8 keys[16];

	int fd = open("/dev/input/event0", O_RDONLY);
	if (fd != -1)
	{
		::memset(&keys[0], 0, sizeof(keys));

		if (ioctl(fd, EVIOCGKEY(sizeof keys), &keys) != -1)
		{
			result = true;

//			while (testChars.isEmpty() == false)
//			{
//				char testChar;

//				testChar = testChars.pop();

//				int arrayIndex = testChar / 8;
//				int positionIndex = testChar % 8;

//                bool bitSet = keys[arrayIndex] & (1 << positionIndex);
//			}

			return true;
		}

		close(fd);
	}
#endif

	return result;
}
