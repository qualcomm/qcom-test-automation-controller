// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// CSV handling based on RFC 4108

#include "CSV.h"

#include <ostream>
#include <string>

static void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty()) return;
	size_t startPos = 0;
	while ((startPos = str.find(from, startPos)) != std::string::npos)
	{
		str.replace(startPos, from.length(), to);
		startPos += to.length();
	}
}

void writeCSVLine
(
	std::ostream& stream,
	const std::vector<std::string>& values
)
{
	bool first = true;

	for (const auto& val : values)
	{
		std::string writeValue = val;

		replaceAll(writeValue, ",", "%x2C");
		replaceAll(writeValue, "\r", "%x0D");
		replaceAll(writeValue, "\n", "%x0A");
		replaceAll(writeValue, "\"", "%x22");

		if (first)
			first = false;
		else
			stream << " , ";

		stream << "\"" << writeValue << "\"";
	}

	if (!values.empty())
		stream << "\r\n";
}
