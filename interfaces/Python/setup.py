# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause-Clear
# Author: Biswajit Roy <biswroy@qti.qualcomm.com>

from setuptools import find_packages, setup

setup(
    name="TACDev",
    version="1.0.1",
    description="X Platform TACDev",
    author="Biswajit Roy",
    author_email="biswroy@qti.qualcomm.com",
    keywords="TACDev TAC",
    packages=find_packages(exclude=[]),
    license="Qualcomm Inc. Proprietary",
)
