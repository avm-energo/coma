cmake_minimum_required(VERSION 3.14)

project(widgets LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Widgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml Widgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Widgets
  REQUIRED)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/styles/QDarkStyleSheet/qdarkstyle/dark/darkstyle.qrc)
    file(RENAME styles/QDarkStyleSheet/qdarkstyle/dark/style.qrc
         styles/QDarkStyleSheet/qdarkstyle/dark/darkstyle.qrc)
endif()
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/styles/QDarkStyleSheet/qdarkstyle/light/lightstyle.qrc)
    file(RENAME styles/QDarkStyleSheet/qdarkstyle/light/style.qrc
         styles/QDarkStyleSheet/qdarkstyle/light/lightstyle.qrc)
endif()
add_library(
  ${PROJECT_NAME} STATIC
  aboutwidget.cpp
  aboutwidget.h
  alarmbutton.cpp
  alarmbutton.h
  alarmwidget.cpp
  alarmwidget.h
  basespinboxgroup.cpp
  basespinboxgroup.h
  checkboxgroup.cpp
  checkboxgroup.h
  checkboxgroup_p.cpp
  checkboxgroup_p.h
  coma.cpp
  coma.h
  ecombobox.cpp
  ecombobox.h
  edoublespinbox.cpp
  edoublespinbox.h
  errorprotocolwidget.cpp
  errorprotocolwidget.h
  etableview.cpp
  etableview.h
  ipctrl.cpp
  ipctrl.h
  passwordlineedit.cpp
  passwordlineedit.h
  signalchoosewidget.cpp
  signalchoosewidget.h
  splashscreen.cpp
  splashscreen.h
  styleloader.cpp
  styleloader.h
  udialog.cpp
  udialog.h
  uwidget.cpp
  uwidget.h
  waitwidget.cpp
  waitwidget.h
  wd_func.cpp
  wd_func.h
  helper.h
  QProgressIndicator.cpp
  QProgressIndicator.h
  ../../images/vectorIcons.qrc
  styles/QDarkStyleSheet/qdarkstyle/light/lightstyle.qrc
  styles/QDarkStyleSheet/qdarkstyle/dark/darkstyle.qrc
  styles/styles.qrc)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE avtuk
  PRIVATE module
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  PRIVATE QXlsx
  PRIVATE CONAN_PKG::boost
  )

#add_dependencies(${PROJECT_NAME} QXlsxBuild)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
