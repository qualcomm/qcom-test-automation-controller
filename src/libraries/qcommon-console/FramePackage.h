#ifndef FRAMEPACKAGE_H
#define FRAMEPACKAGE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
class ReceiveInterface;
#include "QCommonConsoleGlobal.h"
#include "SendInterface.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class QCOMMONCONSOLE_EXPORT _FramePackage
{
public:
	_FramePackage() = default;
	~_FramePackage() = default;

	Argument getArgument(uint32_t index)
	{
		Argument result;

		if (_arguments.size() > index)
			result = _arguments.at(index);

		return result;
	}

	std::string                 _lastError;
	std::string					_request;
	uint32_t					_requestHash{0};
	Arguments					_arguments;
	std::string					_synonym;
	std::string					_codedRequest;
	std::vector<std::string>	_responses;
	uint32_t					_packetID{0};
	std::string					_comment;

	bool						_endTransaction{false};
	bool						_valid{true};
	bool						_console{false};
	bool						_shouldStore{false};
	ReceiveInterface*			_recieveInterface{nullptr};
	uint64_t					_tickcount{0};
	uint32_t					_delayInMilliSeconds{0};

private:
	Q_DISABLE_COPY(_FramePackage)
};

typedef std::shared_ptr<_FramePackage> FramePackage;
typedef std::vector<FramePackage> FramePackageList;

#endif // FRAMEPACKAGE_H
