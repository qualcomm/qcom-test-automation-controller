# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause-Clear
# Author: Biswajit Roy <biswroy@qti.qualcomm.com>


TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += libraries applications interfaces

libraries.subdir = src/libraries
applications.subdir = src/applications
interfaces.subdir = interfaces

applications.depends = libraries
interfaces.depends = libraries
examples.depends = libraries interfaces
