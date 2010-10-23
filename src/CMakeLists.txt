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
  listwindow.cpp
  searchdialog.cpp
  searchlistview.cpp
  searchmodel.cpp
  tvdb.cpp
  tvdbcache.cpp
  updateworker.cpp
  updateprogressdialog.cpp
  settings.cpp
  downloadworker.cpp
  showdelegate.cpp
  settingsdialog.cpp
  actiondialog.cpp
  seriesaction.cpp
  mainlistview.cpp
  maintreeview.cpp
  seriesmenus.cpp
  episodedialog.cpp
  showdialog.cpp
  bannerloader.cpp
  tvdbmodel.cpp
  tvdbitem.cpp
  aboutdialog.cpp)

SET(serieswatcher_UIS
  mainwindow.ui
  listwindow.ui
  searchdialog.ui
  updateprogressdialog.ui
  settingsdialog.ui
  actiondialog.ui
  episodedialog.ui
  episodedialogscroll.ui
  showdialog.ui
  showdialogscroll.ui
  aboutdialog.ui)

SET(serieswatcher_MOC_HDRS
  mainwindow.h
  listwindow.h
  searchdialog.h
  searchmodel.h
  searchlistview.h
  updateworker.h
  downloadworker.h
  updateprogressdialog.h
  settings.h
  showdelegate.h
  settingsdialog.h
  actiondialog.h
  mainlistview.h
  maintreeview.h
  seriesaction.h
  seriesmenus.h
  episodedialog.h
  showdialog.h
  bannerloader.h
  tvdbmodel.h
  aboutdialog.h)

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

if (Q_WS_MAEMO_5)
  target_link_libraries(serieswatcher QtMaemo5)
endif()

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
