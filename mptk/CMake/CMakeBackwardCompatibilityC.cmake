# Nothing here yet
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
 ADD_DEFINITIONS(-fPIC)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(CMAKE_GENERATOR MATCHES "Visual Studio 7")
  INCLUDE(CMakeVS7BackwardCompatibility)
  SET(CMAKE_SKIP_COMPATIBILITY_TESTS 1)
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio 7")
IF(CMAKE_GENERATOR MATCHES "Visual Studio 6")
  INCLUDE(CMakeVS6BackwardCompatibility)
  SET(CMAKE_SKIP_COMPATIBILITY_TESTS 1)
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio 6")
INCLUDE (CheckIncludeFile)
  CHECK_INCLUDE_FILE("limits.h"       HAVE_LIMITS_H)
  CHECK_INCLUDE_FILE("fftw3.h" HAVE_FFTW3)
  CHECK_INCLUDE_FILE("${SNDFILE_INCLUDE_DIR}/sndfile.h" HAVE_SNDFILE)
  CHECK_INCLUDE_FILE("assert.h" HAVE_ASSERT_H)
  CHECK_INCLUDE_FILE("inttypes.h" HAVE_INTTYPES_H)
  CHECK_INCLUDE_FILE("unistd.h"       HAVE_UNISTD_H)
  CHECK_INCLUDE_FILE("pthread.h"      HAVE_PTHREAD_H)
  CHECK_INCLUDE_FILE("math.h" HAVE_MATH_H)
  CHECK_INCLUDE_FILE("memory.h" HAVE_MEMORY_H)
  CHECK_INCLUDE_FILE("stdarg.h" HAVE_STDARG_H)
  CHECK_INCLUDE_FILE("stdint.h" HAVE_STDINT_H)
  CHECK_INCLUDE_FILE("stdlib.h" HAVE_STDLIB_H)
  CHECK_INCLUDE_FILE("strings.h" HAVE_STRINGS_H)
  CHECK_INCLUDE_FILE("string.h" HAVE_STRING_H)
  CHECK_INCLUDE_FILE("sys/stat.h" HAVE_SYS_STAT_H)
  CHECK_INCLUDE_FILE("sys/types.h" HAVE_SYS_TYPES_H)
  INCLUDE (CheckIncludeFiles)
  CHECK_INCLUDE_FILES("sys/types.h;sys/prctl.h"    HAVE_SYS_PRCTL_H)
  CHECK_INCLUDE_FILES("stdlib.h;stddef.h;stdarg.h" STDC_HEADERS)
  INCLUDE (CheckTypeSize)
  CHECK_TYPE_SIZE(int      SIZEOF_INT)
  CHECK_TYPE_SIZE(long     SIZEOF_LONG)
  CHECK_TYPE_SIZE("void*"  SIZEOF_VOIDP)
  CHECK_TYPE_SIZE(char     SIZEOF_CHAR)
  CHECK_TYPE_SIZE(short    SIZEOF_SHORT)
  CHECK_TYPE_SIZE(float    SIZEOF_FLOAT)
  CHECK_TYPE_SIZE(double   SIZEOF_DOUBLE)
  CHECK_TYPE_SIZE("unsigned int" SIZEOF_UNSIGNED_INT)
  CHECK_TYPE_SIZE("unsigned long int" SIZEOF_UNSIGNED_LONG_INT)
  CHECK_TYPE_SIZE("unsigned short int" SIZEOF_UNSIGNED_SHORT_INT)
  INCLUDE (FindThreads)

  SET (CMAKE_THREAD_LIBS        "${CMAKE_THREAD_LIBS_INIT}" CACHE STRING 
    "Thread library used.")

  SET (CMAKE_USE_PTHREADS       "${CMAKE_USE_PTHREADS_INIT}" CACHE BOOL
     "Use the pthreads library.")

  SET (CMAKE_USE_WIN32_THREADS  "${CMAKE_USE_WIN32_THREADS_INIT}" CACHE BOOL
       "Use the win32 thread library.")

IF(NOT CMAKE_SKIP_COMPATIBILITY_TESTS)

  INCLUDE (TestBigEndian)
  TEST_BIG_ENDIAN(WORDS_BIGENDIAN)
    
  IF("${X11_X11_INCLUDE_PATH}" MATCHES "^/usr/include$")
    SET (CMAKE_X_CFLAGS "" CACHE STRING "X11 extra flags.")
  ELSE("${X11_X11_INCLUDE_PATH}" MATCHES "^/usr/include$")
    SET (CMAKE_X_CFLAGS "-I${X11_X11_INCLUDE_PATH}" CACHE STRING
         "X11 extra flags.")
  ENDIF("${X11_X11_INCLUDE_PATH}" MATCHES "^/usr/include$")
  SET (CMAKE_X_LIBS "${X11_LIBRARIES}" CACHE STRING
       "Libraries and options used in X11 programs.")
  SET (CMAKE_HAS_X "${X11_FOUND}" CACHE INTERNAL "Is X11 around.")




ENDIF(NOT CMAKE_SKIP_COMPATIBILITY_TESTS)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/config.h.cmake ${MPTK_BINARY_DIR}/config.h)
MARK_AS_ADVANCED(
CMAKE_THREAD_LIBS
CMAKE_USE_PTHREADS
CMAKE_USE_WIN32_THREADS
CMAKE_X_CFLAGS
CMAKE_X_LIBS
)
