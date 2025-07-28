TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    devlist tacdump ftdi-check updatedevicelist \
    device-catalog test-automation-controller tac-configuration-editor \
    programmers

# configure application locations
devlist.subdir = devlist
tacdump.subdir = tacdump
ftdi-check.subdir = ftdi-check
updatedevicelist.subdir = updatedevicelist

device-catalog.subdir = device-catalog
test-automation-controller.subdir = test-automation-controller
tac-configuration-editor.subdir = tac-configuration-editor

programmers.subdir = programmers
