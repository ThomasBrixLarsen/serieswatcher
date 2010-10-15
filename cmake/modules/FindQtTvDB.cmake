# Find QTTVDB - Access TheTvDB API with Qt
#
# This module defines
#  QTTVDB_FOUND - whether the QtTvDB library was found
#  QTTVDB_LIBRARIES - the QtTvDB library
#  QTTVDB_INCLUDE_DIR - the include path of the QtTvDB library
#

if (QTTVDB_INCLUDE_DIR AND QTTVDB_LIBRARIES)

  # Already in cache
  set (QTTVDB_FOUND TRUE)

else (QTTVDB_INCLUDE_DIR AND QTTVDB_LIBRARIES)

  if (NOT WIN32)
    # use pkg-config to get the values of QTTVDB_INCLUDE_DIRS
    # and QTTVDB_LIBRARY_DIRS to add as hints to the find commands.
    include (FindPkgConfig)
    pkg_check_modules (QTTVDB REQUIRED QtTvDB>=0.1)
  endif (NOT WIN32)

  find_library (QTTVDB_LIBRARIES
    NAMES
    qttvdb
    PATHS
    ${QTTVDB_LIBRARY_DIRS}
    ${LIB_INSTALL_DIR}
    ${KDE4_LIB_DIR}
  )

  find_path (QTTVDB_INCLUDE_DIR
    NAMES
    qttvdb.h
    PATH_SUFFIXES
    qttvdb
    PATHS
    ${QTTVDB_INCLUDE_DIRS}
    ${INCLUDE_INSTALL_DIR}
    ${KDE4_INCLUDE_DIR}
  )

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(QTTVDB DEFAULT_MSG QTTVDB_LIBRARIES QTTVDB_INCLUDE_DIR)

endif (QTTVDB_INCLUDE_DIR AND QTTVDB_LIBRARIES)
