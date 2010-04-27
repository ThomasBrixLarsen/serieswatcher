# Find QUAZIP - WEBDAV handling library for Qt
#
# This module defines
#  QUAZIP_FOUND - whether the qsjon library was found
#  QUAZIP_LIBRARIES - the qwebdav library
#  QUAZIP_INCLUDE_DIR - the include path of the qwebdav library
#

if (QUAZIP_INCLUDE_DIR AND QUAZIP_LIBRARIES)

  # Already in cache
  set (QUAZIP_FOUND TRUE)

else (QUAZIP_INCLUDE_DIR AND QUAZIP_LIBRARIES)

  find_library (QUAZIP_LIBRARIES
    NAMES
    quazip
    PATHS
    ${QUAZIP_LIBRARY_DIRS}
    ${LIB_INSTALL_DIR}
    ${KDE4_LIB_DIR}
  )

  find_path (QUAZIP_INCLUDE_DIR
    NAMES
    quazip.h
    PATH_SUFFIXES
    quazip
    PATHS
    ${QUAZIP_INCLUDE_DIRS}
    ${INCLUDE_INSTALL_DIR}
    ${KDE4_INCLUDE_DIR}
  )

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(QUAZIP DEFAULT_MSG QUAZIP_LIBRARIES QUAZIP_INCLUDE_DIR)

endif (QUAZIP_INCLUDE_DIR AND QUAZIP_LIBRARIES)
