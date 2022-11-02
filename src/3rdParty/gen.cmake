cmake_minimum_required(VERSION 3.15)
include(FetchContent)

FetchContent_Declare(
  gen
  GIT_REPOSITORY https://github.com/vladd12/gen.git
  GIT_TAG        71e8c041de54e358ae849f4ef1b90bcdf226d5dc
)
set(GEN_STATIC ON)
FetchContent_MakeAvailable(gen)
