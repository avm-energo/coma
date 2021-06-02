cmake_minimum_required(VERSION 3.14)

project(grpc_proto C CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../conanbuildinfo.cmake)
include(${CONAN_LIB_DIRS_GRPC}/cmake/grpc_cpp_plugin.cmake)
conan_basic_setup(TARGETS)

find_package(Protobuf REQUIRED)
#find_package(GRPC REQUIRED)
# Proto file
get_filename_component(hw_proto "${CMAKE_CURRENT_SOURCE_DIR}/protos/${PROJECT_NAME}.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)
# Generated sources grpc_cpp_plugin

# Add Library target with protobuf sources

#message(STATUS plugins ${CONAN_BIN_DIRS_GRPC})
#message(STATUS hw_proto ${hw_proto})
#message(STATUS hw_proto_path ${hw_proto_path})
#set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.pb.cc")
#set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.grpc.pb.h")

set(${PROJECT_NAME}_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "GRPC generated files" FORCE)
#set(${PROJECT_NAME}_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} PARENT_SCOPE)
set(hw_grpc_proto_PROTOS ${hw_proto})
message(STATUS grpc-plugin ${grpc_cpp_plugin_location})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${hw_proto})
#grpc_generate_cpp()


message(STATUS ${Protobuf_PROTOC_EXECUTABLE})
add_custom_command(
      OUTPUT "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${Protobuf_PROTOC_EXECUTABLE}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN_PROGRAM}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

add_custom_target(protos_generated DEPENDS ${hw_grpc_srcs} ${hw_grpc_hdrs})
# Include generated *.pb.h files
add_library(${PROJECT_NAME} ${hw_grpc_srcs} ${hw_grpc_hdrs} ${PROTO_SRCS} ${PROTO_HDRS} )
add_dependencies(${PROJECT_NAME} protos_generated)
target_compile_definitions(${PROJECT_NAME} PUBLIC _GLIBCXX_USE_CXX11_ABI=0)
target_compile_definitions(${PROJECT_NAME} PUBLIC gRPC_USE_PROTO_LITE)
#protobuf_generate(TARGET hw_grpc_proto LANGUAGE grpc GENERATE_EXTENSIONS ${hw_grpc_srcs} ${hw_grpc_hdrs} PLUGIN ${GRPC_CPP_PLUGIN_PROGRAM})
# hw_grpc_proto

target_link_libraries(${PROJECT_NAME}
  PUBLIC CONAN_PKG::grpc
  PUBLIC CONAN_PKG::protobuf
  )

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
