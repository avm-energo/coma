cmake_minimum_required(VERSION 3.15)
include(FetchContent)

FetchContent_Declare(
  gen
  GIT_REPOSITORY ssh://git@git.avmenergo.ru:5507/avm-energo/gen.git
  GIT_TAG        origin/main
)
set(GEN_STATIC OFF)
FetchContent_MakeAvailable(gen)
