# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause-Clear
# Author: Biswajit Roy <biswroy@qti.qualcomm.com>

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
	qcommon-console \
	qcommon \
	ui-common

# configure library locations
qcommon-console.subdir = qcommon-console
qcommon.subdir = qcommon
ui-common.subdir = ui-common

# configure library dependencies
qcommon.depends = qcommon-console
ui-common.depends = qcommon-console qcommon
