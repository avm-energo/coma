cmake_minimum_required(VERSION 3.15)
include(FetchContent)

FetchContent_Declare(
  gen
  GIT_REPOSITORY ssh://git@git.avmenergo.ru:5507/avm-energo/gen.git
  GIT_TAG        43d575657f7ed9571867f69a18d74b003701493f
)
set(GEN_STATIC ON)
FetchContent_MakeAvailable(gen)
