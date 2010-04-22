cmake_minimum_required(VERSION 2.6)

project(serieswatcher)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "0")
set(VERSION_PATCH "1")

if(CMAKE_SYSTEM_NAME MATCHES Linux)
  set(CMAKE_MODULE_PATH /usr/share/apps/cmake/modules/)
endif()

find_package(Qt4 REQUIRED)
find_package(QTTVDB REQUIRED)

## GCC  ##
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -ggdb3 -DDEBUG=1")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wstrict-prototypes -Werror-implicit-function-declaration -Wno-pointer-sign")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb3 -DDEBUG=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_definitions(-include ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# compile in debug mode
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if("${CMAKE_BUILD_TYPE}" MATCHES "^Rel.*")
  add_definitions("-DQT_NO_DEBUG_OUTPUT")
endif("${CMAKE_BUILD_TYPE}" MATCHES "^Rel.*")


#add extra search paths for libraries and includes
set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Directory where lib will install")
set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE STRING "Directory where binary will install")
set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE STRING "Directory where include will install")


set (SERIES_WATCHER_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

add_subdirectory(src)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake config.h ESCAPE_QUOTES)

## Package ##
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Series Watcher")
set(CPACK_PACKAGE_VENDOR "iksaif.net")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_CONTACT "corentincj@iksaif.net")
set(CPACK_SOURCE_IGNORE_FILES "~$" "^${PROJECT_SOURCE_DIR}/\\\\.git" "^${PROJECT_SOURCE_DIR}/build/")
if(WIN32)
  # set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/res/serieswatcher.ico")
  set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/res/serieswatcher.ico")
  set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/res/package.bmp")
  set(CPACK_BINARY_NSIS "ON")
  set(CPACK_BINARY_CYGWIN "ON")
  set(CPACK_SOURCE_CYGWIN "OFF")
  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} Series Watcher")
  set(CPACK_NSIS_HELP_LINK "http://iksaif.net/dev/serieswatcher/")
  set(CPACK_NSIS_URL_INFO_ABOUT "http://iksaif.net/dev/serieswatcher/")
  set(CPACK_NSIS_CONTACT "corentincj@iksaif.net")
  set(CPACK_NSIS_MODIFY_PATH ON)
else(WIN32)
  set(CPACK_GENERATOR "DEB;TGZ;TBZ2")
  set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
endif(WIN32)
include(CPack)
