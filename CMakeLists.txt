cmake_minimum_required(VERSION 2.6)

project(serieswatcher)

include(CMakeDependentOption)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_PATCH "3")

if(CMAKE_SYSTEM_NAME MATCHES Linux)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/share/apps/cmake/modules/)
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/)

find_package(Qt4 REQUIRED)
find_package(QtTvDB REQUIRED)
find_package(Quazip)
find_package(LibZip)

include(serieswatcher)

set(HAVE_QUAZIP ${QUAZIP_FOUND})
set(HAVE_LIBZIP ${LIBZIP_FOUND})

if(HAVE_LIBZIP)
  set(ZIP_INCLUDE_DIR ${LIBZIP_INCLUDE_DIR})
  set(ZIP_LIBRARIES   ${LIBZIP_LIBRARIES})
elseif(HAVE_QUAZIP)
  set(ZIP_INCLUDE_DIR ${QUAZIP_INCLUDE_DIR})
  set(ZIP_LIBRARIES   ${QUAZIP_LIBRARIES})
endif()

if(HAVE_LIBZIP OR HAVE_QUAZIP)
  set(HAVE_ZIP 1)
endif()

cmake_dependent_option(WITH_ZIP "With zip support [default: on]" ON "HAVE_ZIP" OFF)
option(WITH_BUILTIN_ICONS  "Include icon theme in built-in ressources" ON)
option(SERIESWATCHER_MEEGO "build for MeeGo [default: off]"  OFF)

## GCC  ##
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -ggdb3 -DDEBUG=1")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wstrict-prototypes -Werror-implicit-function-declaration -Wno-pointer-sign")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb3 -DDEBUG=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if(NOT EXECUTABLE_OUTPUT_PATH)
  set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE STRING
    "Single output directory for building all executables."
    FORCE)
endif()

if(NOT LIBRARY_OUTPUT_PATH)
  set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE STRING
    "Single output directory for building all libraries."
    FORCE)
endif()

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
set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE STRING "Directory where include will install")

if (APPLE)
   set(BIN_INSTALL_DIR "serieswatcher.app/Contents/MacOS")
elseif (WIN32)
   set(BIN_INSTALL_DIR "bin") # FIXME CMake bug
else()
   set(BIN_INSTALL_DIR "bin")
endif()

if (SERIESWATCHER_MEEGO)
   SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
   SET(CMAKE_INSTALL_RPATH "/opt/net.iksaif/serieswatcher/lib")
endif ()

set (SERIES_WATCHER_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

check_symbol_exists(Q_WS_MAEMO_5 "QtCore/qglobal.h" Q_WS_MAEMO_5)

add_subdirectory(common)
add_subdirectory(ui)
add_subdirectory(res)
add_subdirectory(pkg)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake config.h ESCAPE_QUOTES)

## Package ##

# Bundle only accept .txt files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/COPYING ${CMAKE_CURRENT_BINARY_DIR}/COPYING.txt)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/README ${CMAKE_CURRENT_BINARY_DIR}/README.txt)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Series Watcher")
set(CPACK_PACKAGE_VENDOR "iksaif.net")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_VERSION ${SERIES_WATCHER_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_BINARY_DIR}/README.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_BINARY_DIR}/README.txt")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_BINARY_DIR}/README.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_BINARY_DIR}/COPYING.txt")
set(CPACK_PACKAGE_CONTACT "Corentin Chary <corentin.chary@gmail.com>")
set(CPACK_PACKAGE_VENDOR "http://xf.iksaif.net/dev/serieswatcher.html")
set(CPACK_SOURCE_IGNORE_FILES
  "~$"
  "^${PROJECT_SOURCE_DIR}/\\\\.git"
  "^${PROJECT_SOURCE_DIR}/qttvdb/\\\\.git"
  "^${PROJECT_SOURCE_DIR}/build/")

if(WIN32)
  set(CPACK_PACKAGE_FILE_NAME "serieswatcher-${CPACK_VERSION}-win32")
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "serieswatcher")
  set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/res/serieswatcher.ico")
  set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/res/serieswatcher.ico")

  set(CPACK_PACKAGE_EXECUTABLES "serieswatcher" "Series Watcher")

  set(CPACK_BINARY_NSIS "ON")
  set(CPACK_NSIS_MODIFY_PATH "OFF")
  set(CPACK_BINARY_CYGWIN "OFF")
  set(CPACK_SOURCE_CYGWIN "OFF")
  set(CPACK_NSIS_DISPLAY_NAME "Series Watcher")
  set(CPACK_NSIS_HELP_LINK "http://xf.iksaif.net/dev/serieswatcher.html")
  set(CPACK_NSIS_URL_INFO_ABOUT "http://xf.iksaif.net/dev/serieswatcher.html")
  set(CPACK_NSIS_CONTACT "Corentin Chary <corentin.chary@gmail.com>")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "serieswatcher")

elseif(APPLE)
  set(CPACK_GENERATOR "DragNDrop")

  set(CPACK_PACKAGE_FILE_NAME "serieswatcher-${CPACK_VERSION}-osx")
  set(CPACK_PACKAGE_ICON ${CMAKE_SOURCE_DIR}/serieswatcher/res/serieswatcher.icns)

  set(CPACK_BUNDLE_ICON ${CMAKE_SOURCE_DIR}/serieswatcher/res/serieswatcher.icns)
  set(CPACK_BUNDLE_NAME "serieswatcher")
  set(CPACK_BUNDLE_STARTUP_COMMAND "${CMAKE_SOURCE_DIR}/pkg/osx/serieswatcher-startup")
  set(CPACK_BUNDLE_PLIST ${CMAKE_BINARY_DIR}/Info.plist)

  #set(CPACK_DMG_BACKGROUND_IMAGE pkg/osx/background.png)
  #set(CPACK_DMG_DS_STORE pkg/osx/DS_Store)

  set(MACOSX_BUNDLE_GUI_IDENTIFIER "http://xf.iksaif.net/dev/serieswatcher.html")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "serieswatcher" ${CPACK_VERSION})
  set(MACOSX_BUNDLE_BUNDLE_NAME "serieswatcher")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${CPACK_VERSION})
  set(MACOSX_BUNDLE_BUNDLE_VERSION ${CPACK_VERSION})
  set(MACOSX_BUNDLE_COPYRIGHT "Copyright 2010-2011 Corentin Chary")
  set(MACOSX_BUNDLE_INFO_STRING "Lugdulo'V - Find a bike in your city")
  set(MACOSX_BUNDLE_ICON_FILE "serieswatcher.icns")
  set(MACOSX_BUNDLE_EXECUTABLE_NAME "${CMAKE_SOURCE_DIR}/pkg/osx/serieswatcher-startup")
  configure_file(pkg/osx/Info.plist.in ${CMAKE_BINARY_DIR}/Info.plist)
  set(MACOSX_BUNDLE_INFO_PLIST ${CMAKE_BINARY_DIR}/Info.plist)

  install(FILES ${CMAKE_BINARY_DIR}/Info.plist DESTINATION serieswatcher.app/Contents/)
  install(FILES serieswatcher/res/serieswatcher.icns DESTINATION serieswatcher.app/Contents/Resources)

  set(CPACK_BINARY_DRAGNDROP ON)
else()
  set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")

  if (SERIESWATCHER_MEEGO)
    set(CPACK_PACKAGE_FILE_NAME "net.iksaif.serieswatcher-${CPACK_VERSION}-${CMAKE_SYSTEM_NAME}")
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "net.iksaif.serieswatcher-${CPACK_VERSION}")
  else()
    set(CPACK_PACKAGE_FILE_NAME "serieswatcher-${CPACK_VERSION}-${CMAKE_SYSTEM_NAME}")
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "serieswatcher-${CPACK_VERSION}")
  endif()
endif()

include(CPack)
