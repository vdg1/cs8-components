SUBDIRS = lib errorLogBin
TEMPLATE = subdirs
CONFIG += ordered

lib.SUBDIR=lib
errorLogBin.depends=lib

