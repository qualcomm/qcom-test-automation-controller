#!/bin/sh

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

rm -rf TACDev.egg-info
rm -rf build
rm -rf dist

pip3 uninstall -y TACDev
python3 setup.py install
