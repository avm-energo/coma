file(GLOB_RECURSE ctti_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_library(ctti INTERFACE)

target_include_directories(ctti INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

add_library(ctti::ctti ALIAS ctti)
