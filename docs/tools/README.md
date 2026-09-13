# QTAC documentation build

This directory builds the QTAC HTML documentation from the markdown sources in
`docs/bootcamp`, `docs/getting-started`, and `docs/resources`.

`generate-docs.py` copies those sources plus `conf.py`/`index.rst` into a
temporary directory and runs Sphinx against them, producing HTML under
`_build/html`. The QTAC version shown in the built docs is read from
`src/libraries/qcommon-console/version.h`.

## Running the build

```sh
pip install -r requirements.txt
python generate-docs.py
```

The built HTML is written to `docs/tools/_build/html`.
