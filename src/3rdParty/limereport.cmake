project(limereport)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Xml Widgets
  REQUIRED)

if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/conanbuildinfo.txt" )
    execute_process(COMMAND conan install jom/1.1.3@ -g cmake -g cmake_find_package
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        RESULT_VARIABLE CMD_ERROR)
    message(STATUS "Installing jom")
    message( STATUS "CMD_ERROR:" ${CMD_ERROR})
endif()

find_package(jom REQUIRED)

include(${CMAKE_CURRENT_LIST_DIR}/conanbuildinfo.cmake)

message(STATUS "QMAKE : " ${Qt${QT_VERSION_MAJOR}Core_QMAKE_EXECUTABLE})

set(LIMEREPORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include/LimeReport)
message(STATUS "Limereport directory: " ${LIMEREPORT_DIR})

set(JOM_EXEC ${CONAN_BIN_DIRS_JOM}/jom.exe)
message(STATUS "JOM Location: " ${JOM_EXEC})
execute_process(COMMAND ${JOM_EXEC} /version
    RESULT_VARIABLE CMD_ERROR
       OUTPUT_FILE CMD_OUTPUT)

if (${CMD_ERROR})
    message( STATUS "CMD_ERROR:" ${CMD_ERROR})
    message( STATUS "CMD_OUTPUT:" ${CMD_OUTPUT})
endif()


ExternalProject_Add(LimeReportBuild
    SOURCE_DIR ${LIMEREPORT_DIR}
    CONFIGURE_COMMAND ${Qt${QT_VERSION_MAJOR}Core_QMAKE_EXECUTABLE} ${LIMEREPORT_DIR} "CONFIG+=no_zint" "CONFIG+=no_formdesigner" "CONFIG+=no_embedded_designer"  -recursive -spec win32-msvc
    BUILD_COMMAND ${JOM_EXEC} /NOLOGO -f Makefile release
    INSTALL_COMMAND ${JOM_EXEC} /NOLOGO -f Makefile uninstall
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
include_directories(${LIMEREPORT_INCLUDE_DIRS})

link_directories(${LIMEREPORT_BINARY_DIR})
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
  IMPORTED_LOCATION ${LIMEREPORT_BINARY_DIR}
  INTERFACE_INCLUDE_DIRECTORIES ${LIMEREPORT_INCLUDE_DIRS}
  IMPORTED_IMPLIB ${LIMEREPORT_BINARY_DIR}/limereport.lib)
