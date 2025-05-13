cmake_minimum_required(VERSION 3.14)

project(comaresources
  LANGUAGES CXX
  VERSION 1.0.0)

option(COMARES_STATIC:BOOL "Build resources as static library" OFF)

# Finding QtCore library for using rcc compiler and "qt_add_binary_resources" function
find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core REQUIRED)

# Collecting qrc files
set(files_qrc
  "${CMAKE_CURRENT_SOURCE_DIR}/../../images/images.qrc"
)

# Collecting sources
set(COMARES_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}")
set(COMARES_SRC     "${CMAKE_CURRENT_SOURCE_DIR}/src/")
set(COMARES_SOURCES
  "${COMARES_INCLUDE}/export.h"
  "${COMARES_INCLUDE}/manage.h"
  "${COMARES_SRC}/manage.cpp")

# Create library target
if(COMARES_STATIC)
  add_library(${PROJECT_NAME} STATIC ${COMARES_SOURCES} ${files_qrc})
else()
  add_library(${PROJECT_NAME} SHARED ${COMARES_SOURCES} ${files_qrc})
  target_compile_definitions(${PROJECT_NAME}
    PRIVATE "-DCOMARES_SHARED_EXPORT"
    INTERFACE "-DCOMARES_SHARED_IMPORT")
endif()

# Set project version as library target property
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
# Linking libraries for current target
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
