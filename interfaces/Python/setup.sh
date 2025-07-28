#!/bin/sh

rm -rf TACDev.egg-info
rm -rf build
rm -rf dist

pip3 uninstall -y TACDev
python3 setup.py install
