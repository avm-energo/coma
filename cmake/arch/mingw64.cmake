set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(QT_HOST_PATH "/usr")

set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

# Fedora's mingw64-gcc reports a sysroot of /usr/<prefix>/sys-root, with
# headers/libs actually living one level deeper, under .../sys-root/mingw.
# Debian/Ubuntu's gcc-mingw-w64-x86-64 has no such extra nesting: headers/libs
# live directly under /usr/<prefix>. Detect which layout is in use instead of
# hardcoding one distro's convention.
execute_process(
  COMMAND ${TOOLCHAIN_PREFIX}-gcc -print-sysroot
  OUTPUT_VARIABLE _mingw_sysroot
  OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT _mingw_sysroot)
  set(_mingw_sysroot "/usr/${TOOLCHAIN_PREFIX}")
elseif(IS_DIRECTORY "${_mingw_sysroot}/mingw")
  set(_mingw_sysroot "${_mingw_sysroot}/mingw")
endif()

set(CMAKE_FIND_ROOT_PATH ${_mingw_sysroot})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
