include(FetchContent)

FetchContent_Declare(
  gen
  GIT_REPOSITORY https://github.com/avm-energo/gen.git
  GIT_TAG        origin/alice-buster
)

if(BUILD_WITH_ALISE)
  set(GEN_STATIC ON)
else()
  set(GEN_STATIC OFF)
endif()

FetchContent_MakeAvailable(gen)
