TEMPLATE = app

QT += gui xml network sql

TARGET = serieswatcher
ICON = serieswatcher.svg

DESTDIR = ../bin

include(../serieswatcher.pri)

INCLUDEPATH += ../qmake/ ../qttvdb/

unix {
    suffix = ""
}
win32 {
    CONFIG(debug, debug|release) {
      suffix = debug
    } else {
      suffix = release
    }
}

LIBS += -L../qttvdb/$${suffix} -lqttvdb

HEADERS += aboutdialog.h \
  actiondialog.h \
  bannerloader.h \
  downloadworker.h \
  episodedialog.h \
  job.h \
  listwindow.h \
  mainlistview.h \
  maintreeview.h \
  mainwindow.h \
  schema.h \
  searchdialog.h \
  searchlistview.h \
  searchmodel.h \
  seriesaction.h \
  seriesmenus.h \
  serieswatcher.h \
  settingsdialog.h \
  settings.h \
  showdelegate.h \
  showdialog.h \
  tvdbcache.h \
  tvdb.h \
  tvdbitem.h \
  tvdbmodel.h \
  updateprogressdialog.h \
  updateworker.h

SOURCES +=  aboutdialog.cpp \
  actiondialog.cpp \
  bannerloader.cpp \
  downloadworker.cpp \
  episodedialog.cpp \
  listwindow.cpp \
  main.cpp \
  mainlistview.cpp \
  maintreeview.cpp \
  mainwindow.cpp \
  searchdialog.cpp \
  searchlistview.cpp \
  searchmodel.cpp \
  seriesaction.cpp \
  seriesmenus.cpp \
  settings.cpp \
  settingsdialog.cpp \
  showdelegate.cpp \
  showdialog.cpp \
  tvdbcache.cpp \
  tvdb.cpp \
  tvdbitem.cpp \
  tvdbmodel.cpp \
  updateprogressdialog.cpp \
  updateworker.cpp

TRANSLATIONS = i18n/serieswatcher_fr.ts

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ReadUserData UserEnvironment NetworkServices Location

    vendorinfo = \
    "%{\"Corentin Chary - iksaif.net \"}" \
    ":\"Corentin Chary - iksaif.net \""

    SeriesWatcherDeployment.pkg_prerules = vendorinfo
    #SeriesWatcherDeployment.sources = $(EPOCROOT)\\epoc32\\release\\$(PLATFORM)\\serieswatcher.exe
    #SeriesWatcherDeployment.sources = $${TARGET}.exe
    SeriesWatcherDeployment.path = /sys/bin

    DEPLOYMENT += SeriesWatcherDeployment
}

FORMS += aboutdialog.ui \
         actiondialog.ui \
         episodedialogscroll.ui \
         episodedialog.ui \
         listwindow.ui \
         mainwindow.ui \
         searchdialog.ui \
         settingsdialog.ui \
         showdialogscroll.ui \
         showdialog.ui \
         updateprogressdialog.ui

RESOURCES += serieswatcher.qrc \
              ../i18n/i18n.qrc
