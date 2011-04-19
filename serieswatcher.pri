isEmpty(PREFIX) {
  PREFIX = /usr/local/
}

BINDIR = $${PREFIX}/bin
LIBDIR = $${PREFIX}/lib

VERSION = 0.1.3

CONFIG += qt thread

QT_VERSION = $$[QT_VERSION]
QT_VERSION = $$split(QT_VERSION, ".")
QT_VER_MAJ = $$member(QT_VERSION, 0)
QT_VER_MIN = $$member(QT_VERSION, 1)

QT += gui xml network

maemo5: {
    DEFINES += Q_WS_MAEMO_5
}

symbian: {
    DEFINES += SERIES_WATCHER_VERSION=\"\\\"$${VERSION}\\\"\"
}

!symbian: {
    DEFINES += SERIES_WATCHER_VERSION=\\\"$${VERSION}\\\"
}

