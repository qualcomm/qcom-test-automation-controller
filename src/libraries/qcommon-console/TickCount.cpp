// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TickCount.h"

#include <chrono>
#include <thread>

uint64_t tickCount()
{
	auto now = std::chrono::steady_clock::now();
	return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
}

bool sleepUntil
(
	std::function<bool ()> test,
	uint64_t milliseconds
)
{
	uint64_t then = tickCount() + milliseconds;

	while (then > tickCount())
	{
		if (test())
			return true;

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return false;
}
