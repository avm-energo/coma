project(protocom)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5Core)
find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Xml Test
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Xml Test
  REQUIRED)
add_executable(${PROJECT_NAME} testprotocom.cpp testprotocom.h console.cpp
                               console.h #[[main.cpp]])
target_link_libraries(${PROJECT_NAME} interfaces gen Qt5::Core Qt5::Xml Qt5::Test)
