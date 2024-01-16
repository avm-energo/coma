include(FetchContent)

FetchContent_Declare(
  gen
  GIT_REPOSITORY https://github.com/avm-energo/gen.git
  GIT_TAG        fba9c10d5099fc4d1c6c121f87e6682320e90291 # v1.1.0-rc
)
# Building gen as dynamic library
set(GEN_STATIC OFF)
set(GEN_BUILD_TESTS OFF)
FetchContent_MakeAvailable(gen)

# Create install component for deb packages
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  install(TARGETS gen LIBRARY DESTINATION "${CPACK_PACKAGING_INSTALL_PREFIX}/lib" COMPONENT GEN)
endif()
