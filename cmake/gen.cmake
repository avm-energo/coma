include(FetchContent)

FetchContent_Declare(
  gen
  GIT_REPOSITORY https://github.com/avm-energo/gen.git
  GIT_TAG        6627fb009ddd9a8909820f7169d06f567f36077b # pre-v1.3.0
)
# Building gen as dynamic library
set(GEN_STATIC OFF)
set(GEN_BUILD_TESTS OFF)
FetchContent_MakeAvailable(gen)

# Create install component for deb packages
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  install(TARGETS gen LIBRARY DESTINATION "${CPACK_PACKAGING_INSTALL_PREFIX}/lib" COMPONENT GEN)
endif()
