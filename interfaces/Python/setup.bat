@REM  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
@REM  SPDX-License-Identifier: BSD-3-Clause

DEL /F /Q /S TACDev.egg-info > NUL
RMDIR /Q /S TACDev.egg-info

DEL /F /Q /S build > NUL
RMDIR /Q /S build

DEL /F /Q /S dist > NUL
RMDIR /Q /S dist

pip uninstall -y tacdev

@REM setuptools is not guaranteed to be present (e.g. the Microsoft Store
@REM build of Python does not ship it), and setup.py below imports it
@REM unconditionally. Install it first if missing rather than failing with
@REM "ModuleNotFoundError: No module named 'setuptools'".
python -c "import setuptools" 2>NUL || python -m pip install --user setuptools

@REM Use --user so this installs into the current user's own site-packages.
@REM Store/WindowsApps Python installs are read-only for their own
@REM site-packages (even when running elevated), so a plain
@REM "python setup.py install" fails with "Access is denied" there.
python setup.py install --user
