cmake_minimum_required(VERSION 3.14)

project(widgets LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Widgets Xml
  REQUIRED)

find_package(Boost
    COMPONENTS headers
    REQUIRED)

set(QDARKSTYLESHEET_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/styles/QDarkStyleSheet/qdarkstyle)

if(NOT EXISTS ${QDARKSTYLESHEET_PATH}/dark/darkstyle.qrc)
  file(RENAME ${QDARKSTYLESHEET_PATH}/dark/style.qrc
       ${QDARKSTYLESHEET_PATH}/dark/darkstyle.qrc)
endif()
if(NOT EXISTS ${QDARKSTYLESHEET_PATH}/light/lightstyle.qrc)
  file(RENAME ${QDARKSTYLESHEET_PATH}/light/style.qrc
       ${QDARKSTYLESHEET_PATH}/light/lightstyle.qrc)
endif()

file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

add_library(
  ${PROJECT_NAME} STATIC
  ${files_cpp} ${files_h}
  ../../images/vectorIcons.qrc
  styles/QDarkStyleSheet/qdarkstyle/light/lightstyle.qrc
  styles/QDarkStyleSheet/qdarkstyle/dark/darkstyle.qrc
  styles/styles.qrc)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE dialogs
  PRIVATE oscillograms
  PRIVATE module
  PRIVATE s2
  PRIVATE xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE QXlsx
  PRIVATE Boost::headers)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
