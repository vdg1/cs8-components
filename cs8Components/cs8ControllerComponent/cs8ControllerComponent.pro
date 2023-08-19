SUBDIRS = lib errorLogBin \
    FileManagerTest \
    ftpSync
TEMPLATE = subdirs
CONFIG += ordered

lib.SUBDIR=lib
errorLogBin.depends=lib
FileManagerTest.depends=lib

