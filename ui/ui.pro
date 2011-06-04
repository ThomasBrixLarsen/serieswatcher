TEMPLATE = app

QT += gui xml network sql

TARGET = serieswatcher
ICON = serieswatcher.svg

DESTDIR = ../bin

include(../serieswatcher.pri)

INCLUDEPATH += ../qmake/ ../qttvdb/src/
INCLUDEPATH += ../common/

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

LIBS += -L../bin/$${suffix} -lcommon
LIBS += -L../qttvdb/$${suffix} -lqttvdb

HEADERS += aboutdialog.h \
  actiondialog.h \
  episodedialog.h \
  listwindow.h \
  mainlistview.h \
  maintreeview.h \
  mainwindow.h \
  searchdialog.h \
  searchlistview.h \
  seriesmenus.h \
  settingsdialog.h \
  showdelegate.h \
  showdialog.h \
  updateprogressdialog.h

SOURCES += aboutdialog.cpp \
  actiondialog.cpp \
  episodedialog.cpp \
  listwindow.cpp \
  main.cpp \
  mainlistview.cpp \
  maintreeview.cpp \
  mainwindow.cpp \
  searchdialog.cpp \
  searchlistview.cpp \
  seriesmenus.cpp \
  settingsdialog.cpp \
  showdelegate.cpp \
  showdialog.cpp \
  updateprogressdialog.cpp

TRANSLATIONS = i18n/serieswatcher_fr.ts

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

RESOURCES += ../res/serieswatcher.qrc \
  ../res/icons/icons.qrc \
  ../i18n/i18n.qrc

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
