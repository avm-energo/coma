include(FetchContent)

FetchContent_Declare(
  gen
  GIT_REPOSITORY https://github.com/avm-energo/gen.git
  GIT_TAG        origin/develop
)

set(GEN_STATIC OFF)

FetchContent_MakeAvailable(gen)
