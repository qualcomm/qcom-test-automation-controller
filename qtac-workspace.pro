TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += libraries applications interfaces

libraries.subdir = src/libraries
applications.subdir = src/applications
interfaces.subdir = interfaces

applications.depends = libraries
interfaces.depends = libraries
examples.depends = libraries interfaces
