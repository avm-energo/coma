project(alise VERSION 1.0.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Xml REQUIRED)

if (NOT ( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.txt" AND ${CACHED_PROJECT_TARGET_NAME} STREQUAL ${PROJECT_TARGET_NAME}))
#    execute_process(COMMAND ${CONAN_MAIN_INIT}
#        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..
#        RESULT_VARIABLE CMD_ERROR)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E env
            CONAN_USER_HOME=${USER_DIRECTORY} ${CONAN_EXEC} install . -g cmake -g cmake_find_package -s arch=${CONAN_TARGET_NAME} -s arch_build=${CONAN_TARGET_NAME} -if ${CMAKE_CURRENT_BINARY_DIR} --build=missing
        RESULT_VARIABLE CMD_ERROR
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    message(STATUS "Init conan for " ${PROJECT_TARGET_NAME})
    message(STATUS "CMD_ERROR:" ${CMD_ERROR})

endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")

find_package(ZeroMQ REQUIRED)
find_package(cppzmq REQUIRED)

add_executable(
    ${PROJECT_NAME}
    main.cpp
    timesyncronizer.cpp
    timesyncronizer.h
    helper.h
    zerorunner.cpp
    zerorunner.h
    controller.cpp
    controller.h
    stmbroker.cpp
    stmbroker.h
    zeropublisher.cpp
    zeropublisher.h
    zerosubscriber.cpp
    zerosubscriber.h)
target_include_directories(${PROJECT_NAME} PUBLIC ${CONAN_INCLUDE_DIRS_CPPZMQ})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROTOS_GENERATED_DIR})


message(STATUS ALISE ${protos_DIR})
target_link_libraries(${PROJECT_NAME} interfaces gen protos Qt5::Core Qt5::Xml pthread ZeroMQ::libzmq-static cppzmq::cppzmq)

install(TARGETS ${PROJECT_NAME}
)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "AVM-Energo") #required
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
include(CPack)
