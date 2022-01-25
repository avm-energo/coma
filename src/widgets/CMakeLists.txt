cmake_minimum_required(VERSION 3.14)

project(widgets LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Xml Widgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Xml Widgets
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
  delegate_common.h
  ecombobox.cpp
  ecombobox.h
  edoublespinbox.cpp
  edoublespinbox.h
  epopup.cpp
  epopup.h
  errorprotocolwidget.cpp
  errorprotocolwidget.h
  etableview.cpp
  etableview.h
  flowlayout.cpp
  flowlayout.h
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
  PRIVATE QXlsx
  PRIVATE Boost::headers)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
