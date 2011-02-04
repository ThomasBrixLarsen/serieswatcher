macro(CAR var)
  set(${var} ${ARGV1})
endmacro(CAR)

macro(CDR var junk)
  set(${var} ${ARGN})
endmacro(CDR)

macro(ADD_CONVENIENCE_LIBRARY)
  car(LIBNAME ${ARGN})
  cdr(SRCFILES ${ARGN})

  add_library(${LIBNAME} STATIC ${SRCFILES})
  if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set_target_properties(${LIBNAME} PROPERTIES COMPILE_FLAGS "-fPIC")
  endif()
endmacro()
