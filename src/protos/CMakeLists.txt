cmake_minimum_required(VERSION 3.14)

project(protos CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#conan_basic_setup()
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
    message( STATUS "CMD_ERROR:" ${CMD_ERROR})

endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")

find_package(Protobuf REQUIRED)

include_directories(${CONAN_INCLUDE_DIRS_PROTOBUF})
include(ProtobufGenerateWrapper)

set(PROTO_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/protos)

set(PROTO_SRCS)
set(PROTO_HDRS)
generate_protos(${PROTO_FOLDER})
set(${PROJECT_NAME}_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "Protobuf generated files" FORCE)
message(STATUS PROTOBUF_GENERATED ${PROTOS_GENERATED_DIR})
message(STATUS CONAN_INCLUDE_DIRS_PROTOBUF ${CONAN_INCLUDE_DIRS_PROTOBUF})
message(STATUS ${Protobuf_PROTOC_EXECUTABLE})

message(STATUS "Generated srcs: " ${PROTO_SRCS})
message(STATUS "Generated hdrs: " ${PROTO_HDRS})

add_library(${PROJECT_NAME} ${PROTO_SRCS} ${PROTO_HDRS})

#target_compile_definitions(${PROJECT_NAME} PUBLIC _GLIBCXX_USE_CXX11_ABI=0)

target_link_libraries(${PROJECT_NAME}
  PRIVATE protobuf
  )

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
