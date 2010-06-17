# Find LIBZIP
#
# This module defines
#  LIBZIP_FOUND - whether the qsjon library was found
#  LIBZIP_LIBRARIES - the qwebdav library
#  LIBZIP_INCLUDE_DIR - the include path of the qwebdav library
#

if (LIBZIP_INCLUDE_DIR AND LIBZIP_LIBRARIES)

  # Already in cache
  set (LIBZIP_FOUND TRUE)

else (LIBZIP_INCLUDE_DIR AND LIBZIP_LIBRARIES)

  find_library (LIBZIP_LIBRARIES
    NAMES
    zip
    PATHS
    ${LIBZIP_LIBRARY_DIRS}
    ${LIB_INSTALL_DIR}
  )

  find_path (LIBZIP_INCLUDE_DIR
    NAMES
    zip.h
    PATHS
    ${LIBZIP_INCLUDE_DIRS}
    ${INCLUDE_INSTALL_DIR}
  )

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(LIBZIP DEFAULT_MSG LIBZIP_LIBRARIES LIBZIP_INCLUDE_DIR)

endif (LIBZIP_INCLUDE_DIR AND LIBZIP_LIBRARIES)
