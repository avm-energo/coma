project(xmlParser_test)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Xml Test
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Xml Test
  REQUIRED)

find_package(hidapi REQUIRED)

add_executable(${PROJECT_NAME} testmodule.cpp testmodule.h)
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} gen module
    interfaces
    gen
    widgets
    dialogs
    models
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Xml
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Test)

if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
    get_filename_component(QT_BIN_DIR ${QT_QMAKE_EXECUTABLE} DIRECTORY)
    get_filename_component(hidapi_BIN_DIR ${hidapi_LIB_DIRS}/../bin ABSOLUTE)
    set_tests_properties(
        ${PROJECT_NAME}
    PROPERTIES
        # Make sure DLL is found by adding its directory to PATH
        ENVIRONMENT "PATH=${hidapi_BIN_DIR}\;${QT_BIN_DIR}\;$<TARGET_PROPERTY:QXlsx,IMPORTED_LOCATION>;"
    )

endif()
