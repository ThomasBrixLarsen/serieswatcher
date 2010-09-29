# Use it
set(QT_USE_QTGUI TRUE)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTSQL TRUE)

INCLUDE( ${QT_USE_FILE} )

# Include the library include directories, and the current build directory (moc)
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${serieswatcher_BINARY_DIR}
	${QTTVDB_INCLUDE_DIR}
	${ZIP_INCLUDE_DIR}
)

LINK_DIRECTORIES(
	${QTTVDB_LIBRARY_DIRS}
)

SET(serieswatcher_SRCS
  main.cpp
  mainwindow.cpp
  searchdialog.cpp
  searchlistview.cpp
  searchmodel.cpp
  tvdb.cpp
  tvdbcache.cpp
  updateworker.cpp
  updateprogressdialog.cpp
  settings.cpp
  downloadworker.cpp
  showmodel.cpp
  seasonmodel.cpp
  episodemodel.cpp
  showdelegate.cpp
  settingsdialog.cpp
  actiondialog.cpp
  seriesaction.cpp
  mainlistview.cpp
  maintreewidget.cpp
  seriesmenus.cpp
  episodedialog.cpp
  showdialog.cpp
  bannerloader.cpp)

SET(serieswatcher_UIS
  mainwindow.ui
  searchdialog.ui
  updateprogressdialog.ui
  settingsdialog.ui
  actiondialog.ui
  episodedialog.ui
  showdialog.ui)

SET(serieswatcher_MOC_HDRS
  mainwindow.h
  searchdialog.h
  searchmodel.h
  searchlistview.h
  updateworker.h
  downloadworker.h
  updateprogressdialog.h
  settings.h
  showmodel.h
  seasonmodel.h
  episodemodel.h
  showdelegate.h
  settingsdialog.h
  actiondialog.h
  mainlistview.h
  maintreewidget.h
  seriesaction.h
  seriesmenus.h
  episodedialog.h
  showdialog.h
  bannerloader.h)

SET(serieswatcher_RCS serieswatcher.qrc)

if(WITH_BUILTIN_ICONS)
  SET(serieswatcher_RCS ${serieswatcher_RCS} ../res/icons/icons.qrc)
endif()

QT4_WRAP_CPP(MOC_SOURCE ${serieswatcher_MOC_HDRS})
QT4_WRAP_UI(UI_HDRS ${serieswatcher_UIS})
QT4_ADD_RESOURCES(RC_SRCS ${serieswatcher_RCS})

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
	${ZIP_LIBRARIES}
)

install(TARGETS serieswatcher
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)
