include(FetchContent)

FetchContent_Declare(
  gen
  GIT_REPOSITORY https://github.com/avm-energo/gen.git
  GIT_TAG        origin/feature/debug-loglevel
)
# Building gen as dynamic library
set(GEN_STATIC OFF)
set(GEN_BUILD_TESTS OFF)
set(GEN_USING_SANITIZERS OFF) # Because we already set global sanitizer flags
FetchContent_MakeAvailable(gen)

# Create install component for deb packages
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  install(TARGETS gen LIBRARY DESTINATION "${CPACK_PACKAGING_INSTALL_PREFIX}/lib" COMPONENT GEN)
endif()
