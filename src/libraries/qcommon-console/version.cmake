# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

set(QTAC_VERSION "6.9.4")
set(DEVICE_CATALOG_VERSION "2.0.0")

set(UPDATE_DEVICE_LIST_VERSION 1.0.0)
set(FTDI_CHECK_VERSION "3.0.0")
set(FW_UPDATE_VERSION "3.0.0")

set(QCOMMONCOMMAND_VERSION "2.0.0")
set(EEPROM_COMMAND "2.0.0")

# version.h carries the same QTAC_VERSION for the C++ code (About box, TACDev,
# logs) and for docs/tools/generate-docs.py. The two copies drifted once (6.9.1
# and 6.9.2 shipped reporting 6.9.0), so refuse to configure when they differ.
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/version.h" _qtac_version_h_line
	REGEX "^#define[ \t]+QTAC_VERSION[ \t]+\"[^\"]+\"")
string(REGEX REPLACE ".*\"([^\"]+)\".*" "\\1" _qtac_version_h "${_qtac_version_h_line}")
if(NOT _qtac_version_h STREQUAL QTAC_VERSION)
	message(FATAL_ERROR
		"QTAC_VERSION mismatch: version.cmake has \"${QTAC_VERSION}\" but version.h has "
		"\"${_qtac_version_h}\". Update both files in src/libraries/qcommon-console/.")
endif()

# Directory of this file, captured at include time so functions below can find version_info.rc.in.
set(QTAC_VERSION_INFO_TEMPLATE "${CMAKE_CURRENT_LIST_DIR}/version_info.rc.in")

# Converts "a.b.c" (or "a.b.c.d") to the "a,b,c,d" form used by VERSIONINFO.
function(_qtac_version_to_csv version_str out_var)
	string(REPLACE "." ";" _parts "${version_str}")
	list(LENGTH _parts _count)
	while(_count LESS 4)
		list(APPEND _parts 0)
		math(EXPR _count "${_count} + 1")
	endwhile()
	list(SUBLIST _parts 0 4 _parts)
	string(REPLACE ";" "," _csv "${_parts}")
	set(${out_var} "${_csv}" PARENT_SCOPE)
endfunction()

# Adds a Windows VERSIONINFO resource to an executable or shared library.
# FileVersion is <version>; ProductVersion is QTAC_VERSION. No-op on other platforms.
function(qtac_add_version_info target_name description version)
	if(NOT WIN32)
		return()
	endif()
	get_target_property(_ttype ${target_name} TYPE)
	get_target_property(_outname ${target_name} OUTPUT_NAME)
	if(NOT _outname)
		set(_outname "${target_name}")
	endif()
	if(_ttype STREQUAL "SHARED_LIBRARY")
		set(VI_ORIGINAL_FILENAME "${_outname}.dll")
		set(VI_FILE_TYPE "0x2L")
	else()
		set(VI_ORIGINAL_FILENAME "${_outname}.exe")
		set(VI_FILE_TYPE "0x1L")
	endif()
	_qtac_version_to_csv("${version}"      VI_FILE_VERSION_CSV)
	_qtac_version_to_csv("${QTAC_VERSION}" VI_PRODUCT_VERSION_CSV)
	set(VI_FILE_DESCRIPTION    "${description}")
	set(VI_FILE_VERSION_STR    "${version}")
	set(VI_PRODUCT_VERSION_STR "${QTAC_VERSION}")
	set(VI_INTERNAL_NAME       "${_outname}")
	set(_rc_out "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_version_info.rc")
	configure_file("${QTAC_VERSION_INFO_TEMPLATE}" "${_rc_out}" @ONLY)
	target_sources(${target_name} PRIVATE "${_rc_out}")
endfunction()
