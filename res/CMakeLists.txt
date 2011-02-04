project(resources)

include( ${QT_USE_FILE} )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${serieswatcher_BINARY_DIR}
)

set(resources_RCS serieswatcher.qrc)

if(WITH_BUILTIN_ICONS)
  set(resources_RCS ${resources_RCS} icons/icons.qrc)
endif()

qt4_add_resources(RC_SRCS ${resources_RCS})

if (WIN32)
 add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/icon.o
                     COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/serieswatcher.rc
                     -o ${CMAKE_CURRENT_BINARY_DIR}/icon.o )
 set(resources_SRCS ${resources_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/icon.o)
endif ()

add_convenience_library(resources ${RC_SRCS})

target_link_libraries(resources
  ${QT_LIBRARIES}
)
