project(common)

set(QT_USE_QTGUI TRUE)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTSQL TRUE)

INCLUDE( ${QT_USE_FILE} )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${serieswatcher_BINARY_DIR}
  ${QTTVDB_INCLUDE_DIR}
  ${ZIP_INCLUDE_DIR}
)

link_directories(
  ${QTTVDB_LIBRARY_DIRS}
)

set(common_SRCS
  searchmodel.cpp
  tvdb.cpp
  tvdbcache.cpp
  updateworker.cpp
  settings.cpp
  downloadworker.cpp
  seriesaction.cpp
  tvdbmodel.cpp
  tvdbitem.cpp
  bannerloader.cpp)

set(common_MOC_HDRS
  searchmodel.h
  updateworker.h
  downloadworker.h
  settings.h
  seriesaction.h
  tvdbmodel.h
  bannerloader.h
  tvdbcache.h)

qt4_wrap_cpp(MOC_SOURCE ${common_MOC_HDRS})

add_convenience_library(common
  ${common_SRCS}
  ${MOC_SOURCE}
)

target_link_libraries(common
  ${QT_LIBRARIES}
  ${QTTVDB_LIBRARIES}
  ${ZIP_LIBRARIES}
)
