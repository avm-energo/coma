cmake_minimum_required(VERSION 3.14)

project(dialogs LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Widgets SerialPort Concurrent Svg
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Widgets SerialPort Concurrent Svg
  REQUIRED)

set(SRCS
    abstractinterfacedialog.cpp
    abstractinterfacedialog.h
    connectdialog.cpp
    connectdialog.h
    errordialog.cpp
    errordialog.h
    fwuploaddialog.cpp
    fwuploaddialog.h
    infodialog.cpp
    infodialog.h
    interfaceethernetdialog.cpp
    interfaceethernetdialog.h
    interfaceserialdialog.cpp
    interfaceserialdialog.h
    interfaceusbdialog.cpp
    interfaceusbdialog.h
    journalsdialog.cpp
    journalsdialog.h
    hiddendialog.cpp
    hiddendialog.h
    keypressdialog.cpp
    keypressdialog.h
    settingsdialog.cpp
    settingsdialog.h
    timedialog.cpp
    timedialog.h
    plotdialog.cpp
    plotdialog.h)

if (ENABLE_EMULATOR)
    list(APPEND SRCS interfaceemudialog.cpp interfaceemudialog.h)
endif()

add_library(
  ${PROJECT_NAME} STATIC
  ${SRCS}
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE interfaces
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort
  PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent
  PRIVATE Qt${QT_VERSION_MAJOR}::Svg
  PRIVATE QXlsx)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)

if (ENABLE_EMULATOR)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_EMULATOR)
endif()
