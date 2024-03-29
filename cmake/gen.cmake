include(FetchContent)

FetchContent_Declare(
  gen
  GIT_REPOSITORY https://github.com/avm-energo/gen.git
  GIT_TAG        f2d5f044be3551bf331e79b42d143fcd230ebdd7 # v1.3.0-rc1
)
# Building gen as dynamic library
set(GEN_STATIC OFF)
set(GEN_BUILD_TESTS OFF)
FetchContent_MakeAvailable(gen)

# Create install component for deb packages
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  install(TARGETS gen LIBRARY DESTINATION "${CPACK_PACKAGING_INSTALL_PREFIX}/lib" COMPONENT GEN)
endif()
