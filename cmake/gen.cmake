include(FetchContent)

FetchContent_Declare(
  gen
  GIT_REPOSITORY https://github.com/avm-energo/gen.git
  GIT_TAG        a5c96c7fd329d3e7e02acd789acbfb12c4293216 # v1.1.0
)
# Building gen as dynamic library
set(GEN_STATIC OFF)
FetchContent_MakeAvailable(gen)

# Create install component for deb packages
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  install(TARGETS gen LIBRARY DESTINATION "${CPACK_PACKAGING_INSTALL_PREFIX}/lib" COMPONENT GEN)
endif()
