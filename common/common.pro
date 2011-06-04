TEMPLATE = lib
CONFIG += staticlib
QT += gui xml network sql
TARGET = common
DESTDIR = ../bin

include(../serieswatcher.pri)

INCLUDEPATH += ../qmake/ ../qttvdb/src/

LIBS += -L../qttvdb/$${suffix} -lqttvdb

HEADERS += bannerloader.h \
  downloadworker.h \
  job.h \
  schema.h \
  searchmodel.h \
  seriesaction.h \
  serieswatcher.h \
  settings.h \
  tvdbcache.h \
  tvdb.h \
  tvdbitem.h \
  tvdbmodel.h \
  updateworker.h


SOURCES +=  bannerloader.cpp \
  downloadworker.cpp \
  searchmodel.cpp \
  seriesaction.cpp \
  settings.cpp \
  tvdbcache.cpp \
  tvdb.cpp \
  tvdbitem.cpp \
  tvdbmodel.cpp \
  updateworker.cpp
