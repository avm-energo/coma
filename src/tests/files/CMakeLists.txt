project(files_test)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(QT NAMES Qt6 Qt5)
find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Xml Widgets Test
  REQUIRED)

add_executable(${PROJECT_NAME}
  testfiles.cpp
  testfiles.h
  examples.qrc)
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
  gen
  oscillograms
  widgets
  dialogs
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Xml
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Test)
