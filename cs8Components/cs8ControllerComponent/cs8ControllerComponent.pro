SUBDIRS = lib errorLogBin \
    FileManagerTest
TEMPLATE = subdirs
CONFIG += ordered

lib.SUBDIR=lib
errorLogBin.depends=lib
FileManagerTest.depends=lib

