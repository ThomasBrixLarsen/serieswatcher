# Use it
set(QT_USE_QTGUI TRUE)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)

INCLUDE( ${QT_USE_FILE} )

# Include the library include directories, and the current build directory (moc)
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${serieswatcher_BINARY_DIR}
	${QTTVDB_INCLUDE_DIR}
)

SET(serieswatcher_SRCS main.cpp mainwindow.cpp searchdialog.cpp tvdb.cpp searchlistwidget.cpp)
SET(serieswatcher_UIS mainwindow.ui searchdialog.ui)
SET(serieswatcher_MOC_HDRS mainwindow.h searchdialog.h searchlistwidget.h)
SET(serieswatcher_RCS serieswatcher.qrc)

QT4_WRAP_CPP(MOC_SOURCE ${serieswatcher_MOC_HDRS})
QT4_WRAP_UI(UI_HDRS ${serieswatcher_UIS})
QT4_ADD_RESOURCES(RC_SRCS ${serieswatcher_RCS} )

ADD_EXECUTABLE(
	serieswatcher
	${serieswatcher_SRCS}
	${UI_HDRS}
	${MOC_SOURCE}
	${RC_SRCS}
)

TARGET_LINK_LIBRARIES(
	serieswatcher
	${QT_LIBRARIES}
	${QTTVDB_LIBRARIES}
)

