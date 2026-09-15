# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.

"""
Builds the QTAC HTML documentation from the markdown sources in
docs/bootcamp, docs/getting-started and docs/resources.
"""

import re
import tempfile
from pathlib import Path
from shutil import copyfile, copytree

from sphinx.application import Sphinx

TOOLS_DIR = Path(__file__).resolve().parent
VERSION_FILE = TOOLS_DIR.parents[1] / "src" / "libraries" / "qcommon-console" / "version.h"
DOCUMENTATION_DIRS = ["getting-started", "bootcamp", "resources"]
OUTPUT_DIR = TOOLS_DIR / "_build" / "html"


def get_qtac_version() -> str:
    """
    Extracts the QTAC_VERSION string from version.h so the built documentation
    always matches the version of the software it documents.
    """
    match = re.search(r'QTAC_VERSION\s+"([^"]+)"', VERSION_FILE.read_text(encoding="utf-8"))
    if match is None:
        raise RuntimeError(f"Could not find QTAC_VERSION in {VERSION_FILE}")
    return match.group(1)


def main() -> None:
    with tempfile.TemporaryDirectory() as tmpdir:
        srcdir = Path(tmpdir)

        for template in ("conf.py", "index.rst"):
            copyfile(TOOLS_DIR / template, srcdir / template)

        for name in DOCUMENTATION_DIRS:
            source = TOOLS_DIR.parent / name
            if not source.exists():
                raise FileNotFoundError(f"{source} does not exist!")
            copytree(source, srcdir / name, dirs_exist_ok=True)

        doctree_dir = srcdir / "_doctrees"
        Sphinx(
            srcdir=str(srcdir),
            confdir=str(srcdir),
            outdir=str(OUTPUT_DIR),
            doctreedir=str(doctree_dir),
            buildername="html",
            confoverrides={"version": get_qtac_version(), "release": get_qtac_version()},
        ).build()


if __name__ == "__main__":
    main()
