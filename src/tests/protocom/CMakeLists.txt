project(protocom_test)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Xml Test
  REQUIRED)

add_executable(${PROJECT_NAME}
  testprotocom.cpp
  testprotocom.h
  console.cpp
  console.h)

target_link_libraries(${PROJECT_NAME}
  interfaces
  gen
  s2
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Xml
  Qt${QT_VERSION_MAJOR}::Test)
