include(FetchContent)

FetchContent_Declare(magic-enum
  GIT_REPOSITORY    https://github.com/avm-energo/magic_enum.git
  GIT_TAG           master
)

FetchContent_MakeAvailable(magic-enum)
