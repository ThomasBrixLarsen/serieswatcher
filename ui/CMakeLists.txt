# Use it
set(QT_USE_QTGUI TRUE)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTSQL TRUE)

include( ${QT_USE_FILE} )

# Include the library include directories, and the current build directory (moc)
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${serieswatcher_BINARY_DIR}
  ${resources_BINARY_DIR}
  ${common_SOURCE_DIR}
  ${common_BINARY_DIR}
  ${QTTVDB_INCLUDE_DIR}
)

link_directories(
  ${QTTVDB_LIBRARY_DIRS}
)

set(serieswatcher_SRCS
  main.cpp
  mainwindow.cpp
  listwindow.cpp
  searchdialog.cpp
  searchlistview.cpp
  updateprogressdialog.cpp
  showdelegate.cpp
  settingsdialog.cpp
  actiondialog.cpp
  mainlistview.cpp
  maintreeview.cpp
  seriesmenus.cpp
  episodedialog.cpp
  showdialog.cpp
  aboutdialog.cpp)

set(serieswatcher_UIS
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
  searchlistview.h
  updateprogressdialog.h
  showdelegate.h
  settingsdialog.h
  actiondialog.h
  mainlistview.h
  maintreeview.h
  seriesmenus.h
  episodedialog.h
  showdialog.h
  aboutdialog.h)

qt4_wrap_cpp(MOC_SOURCE ${serieswatcher_MOC_HDRS})
qt4_wrap_ui(UI_HDRS ${serieswatcher_UIS})

if (WIN32)
 set(GUI_TYPE WIN32)
endif ()

add_executable(
  serieswatcher
  ${GUI_TYPE}
  ${serieswatcher_SRCS}
  ${UI_HDRS}
  ${MOC_SOURCE}
)

if (Q_WS_MAEMO_5)
  target_link_libraries(serieswatcher QtMaemo5)
endif()

target_link_libraries(
  serieswatcher
  resources
  common
  ${QT_LIBRARIES}
  ${QTTVDB_LIBRARIES}
  ${ZIP_LIBRARIES}
)

install(TARGETS serieswatcher
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)
