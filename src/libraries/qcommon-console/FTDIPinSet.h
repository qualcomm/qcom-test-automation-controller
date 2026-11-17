#ifndef FTDIPINSET_H
#define FTDIPINSET_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <cstdint>

const int kMaxPinSetCount{4};

enum FTDIPinSet : uint32_t
{
	NoOptions = 0x0,
	eA = 0x1,
	eB = 0x2,
	eC = 0x4,
	eD = 0x8
};

class FTDIPinSets
{
public:
	constexpr FTDIPinSets() : _flags(0) {}
	constexpr FTDIPinSets(FTDIPinSet set) : _flags(static_cast<uint32_t>(set)) {}
	constexpr FTDIPinSets(uint32_t flags) : _flags(flags) {}

	constexpr bool testFlag(FTDIPinSet flag) const
	{
		return (_flags & static_cast<uint32_t>(flag)) == static_cast<uint32_t>(flag);
	}

	constexpr void setFlag(FTDIPinSet flag, bool on = true)
	{
		if (on)
			_flags |= static_cast<uint32_t>(flag);
		else
			_flags &= ~static_cast<uint32_t>(flag);
	}

	constexpr operator uint32_t() const { return _flags; }

	constexpr FTDIPinSets& operator|=(FTDIPinSet flag)
	{
		_flags |= static_cast<uint32_t>(flag);
		return *this;
	}

	constexpr FTDIPinSets& operator|=(FTDIPinSets other)
	{
		_flags |= other._flags;
		return *this;
	}

	friend constexpr FTDIPinSets operator|(FTDIPinSets a, FTDIPinSet b)
	{
		return FTDIPinSets(a._flags | static_cast<uint32_t>(b));
	}

	friend constexpr FTDIPinSets operator|(FTDIPinSet a, FTDIPinSet b)
	{
		return FTDIPinSets(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	friend constexpr FTDIPinSets operator|(FTDIPinSets a, FTDIPinSets b)
	{
		return FTDIPinSets(a._flags | b._flags);
	}

	friend constexpr bool operator==(FTDIPinSets a, FTDIPinSets b)
	{
		return a._flags == b._flags;
	}

private:
	uint32_t _flags{0};
};

#endif // FTDIPINSET_H
