
find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Xml Widgets
  REQUIRED)

set(LIMEREPORT_BUILD_DIR ${CMAKE_BINARY_DIR}/LimeReportBuild)


if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
    get_filename_component(_fullpath "${LIMEREPORT_BUILD_DIR}/conanbuildinfo.cmake" REALPATH)
    set(CONAN_JOM_INIT ${CONAN_EXEC} install jom/1.1.3@ -g cmake -g cmake_find_package -s arch=${CONAN_TARGET_NAME} -s arch_build=${CONAN_TARGET_NAME} -if ${LIMEREPORT_BUILD_DIR})
    if (NOT ( EXISTS "${_fullpath}" AND ${CACHED_PROJECT_TARGET_NAME} STREQUAL ${PROJECT_TARGET_NAME}))
    #    execute_process(COMMAND ${CONAN_JOM_INIT}
    #        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    #        RESULT_VARIABLE CMD_ERROR)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E env
                CONAN_USER_HOME=${USER_DIRECTORY} ${CONAN_JOM_INIT}
            RESULT_VARIABLE CMD_ERROR
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        )
        message(STATUS "Cannot find jom: " ${_fullpath})
        message(STATUS "Installing jom")
        message(STATUS "CMD_ERROR:" ${CMD_ERROR})
    endif()
    list(APPEND CMAKE_MODULE_PATH "${LIMEREPORT_BUILD_DIR}")
    find_package(jom REQUIRED)
    include(${LIMEREPORT_BUILD_DIR}/conanbuildinfo.cmake)
    set(JOM_EXEC ${CONAN_BIN_DIRS_JOM}/jom.exe)
    message(STATUS "JOM Location: " ${JOM_EXEC})
    execute_process(COMMAND ${JOM_EXEC} /version
        RESULT_VARIABLE CMD_ERROR
        OUTPUT_FILE CMD_OUTPUT)

    if (${CMD_ERROR})
        message( STATUS "CMD_ERROR:" ${CMD_ERROR})
        message( STATUS "CMD_OUTPUT:" ${CMD_OUTPUT})
    endif()
    set(LIMEREPORT_BUILD_COMMAND ${JOM_EXEC} /NOLOGO -f Makefile release)
    set(LIMEREPORT_INSTALL_COMMAND ${JOM_EXEC} /NOLOGO -f Makefile uninstall)
    set(LIMEREPORT_SPEC win32-msvc)

else()
    find_program(MAKE_EXEC "make")
    if(NOT MAKE_EXEC)
        message(STATUS "No make found, install build-essential")
    endif()
    set(LIMEREPORT_SPEC "linux-g++")
    if (NOT LIMEREPORT_THREADS)
        set(LIMEREPORT_THREADS 16)
    endif()
    set(LIMEREPORT_BUILD_COMMAND ${MAKE_EXEC} -j ${LIMEREPORT_THREADS} sub-limereport)
    set(LIMEREPORT_INSTALL_COMMAND echo "No install : )")
endif()



message(STATUS "QMAKE : " ${Qt${QT_VERSION_MAJOR}Core_QMAKE_EXECUTABLE})

set(LIMEREPORT_DIR ${CMAKE_SOURCE_DIR}/../include/LimeReport)
message(STATUS "Limereport directory: " ${LIMEREPORT_DIR})





ExternalProject_Add(LimeReportBuild
    BINARY_DIR ${LIMEREPORT_BUILD_DIR}
    SOURCE_DIR ${LIMEREPORT_DIR}
    CONFIGURE_COMMAND ${Qt${QT_VERSION_MAJOR}Core_QMAKE_EXECUTABLE} ${LIMEREPORT_DIR} "CONFIG+=no_zint" "CONFIG+=no_formdesigner" "CONFIG+=no_embedded_designer"  -recursive -spec ${LIMEREPORT_SPEC}
    BUILD_COMMAND ${LIMEREPORT_BUILD_COMMAND}
    INSTALL_COMMAND ${LIMEREPORT_INSTALL_COMMAND}
    BUILD_BYPRODUCTS ${LIMEREPORT_DIR}/build/${Qt${QT_VERSION_MAJOR}Core_VERSION_STRING}/${PROJECT_TARGET_NAME}/release/lib/limereport.lib
    USES_TERMINAL_BUILD TRUE
    USES_TERMINAL_CONFIGURE TRUE)

ExternalProject_Get_property(LimeReportBuild SOURCE_DIR)
message(STATUS "Source dir of LimeReport = ${SOURCE_DIR}")

set (LIMEREPORT_BINARY_DIR ${SOURCE_DIR}/build/${Qt${QT_VERSION_MAJOR}Core_VERSION_STRING}/${PROJECT_TARGET_NAME}/release/lib)
message(STATUS "Binary dir of LimeReport = ${LIMEREPORT_BINARY_DIR}")

get_filename_component(_fullpath "${LIMEREPORT_BINARY_DIR}" REALPATH)
if (NOT EXISTS "${_fullpath}" )
    message(STATUS "LimeReport directory does not exist")
    file(MAKE_DIRECTORY ${_fullpath})
    message(STATUS "Created directory: " ${_fullpath})
endif()
# set the include directory variable and include it
set(LIMEREPORT_INCLUDE_DIRS ${LIMEREPORT_BINARY_DIR}/include)
get_filename_component(_fullpath "${LIMEREPORT_INCLUDE_DIRS}" REALPATH)
if (NOT EXISTS "${_fullpath}" )
    message(STATUS "LimeReport include directory does not exist")
    file(MAKE_DIRECTORY ${_fullpath})
    message(STATUS "Created directory: " ${_fullpath})
endif()
#include_directories(${LIMEREPORT_INCLUDE_DIRS})

#link_directories(${LIMEREPORT_BINARY_DIR})
set(LIMEREPORT_LIBS limereport)
set(LIMEREPORT_LIBRARY_DIRS ${LIMEREPORT_BINARY_DIR})

# verify that the TBB header files can be included
set(CMAKE_REQUIRED_INCLUDES_SAVE ${CMAKE_REQUIRED_INCLUDES})
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} 	${LIMEREPORT_INCLUDE_DIRS})

# TODO FIX false negative
check_include_file_cxx("lrreportengine.h" HAVE_LIMEREPORT)
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_SAVE})
 if (NOT HAVE_LIMEREPORT)
        message(STATUS "Did not build LIMEREPORT correctly as cannot find lrreportengine.h. Will build it.")
        set(HAVE_LIMEREPORT 1)
endif (NOT HAVE_LIMEREPORT)

# Hack to make it work, otherwise INTERFACE_INCLUDE_DIRECTORIES will not be propagated
get_filename_component(_fullpath "${LIMEREPORT_BINARY_DIR}" REALPATH)
if (NOT EXISTS ${_fullpath})
file(MAKE_DIRECTORY ${_fullpath})
message(STATUS "Created directory: " ${_fullpath})
endif()
get_filename_component(_fullpath "${LIMEREPORT_INCLUDE_DIRS}" REALPATH)
if (NOT EXISTS ${_fullpath})
file(MAKE_DIRECTORY ${_fullpath})
message(STATUS "Created directory: " ${_fullpath})
endif()

add_library(limereport SHARED IMPORTED GLOBAL)
set_target_properties(limereport PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${LIMEREPORT_INCLUDE_DIRS})
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
    set_target_properties(limereport PROPERTIES
      IMPORTED_LOCATION ${LIMEREPORT_BINARY_DIR}
      IMPORTED_IMPLIB ${LIMEREPORT_BINARY_DIR}/limereport.lib)
else()
    set_target_properties(limereport PROPERTIES
      IMPORTED_LOCATION ${LIMEREPORT_BINARY_DIR}/liblimereport.so)
endif()
add_dependencies(limereport LimeReportBuild)
