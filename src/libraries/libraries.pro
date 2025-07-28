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
