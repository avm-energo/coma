include(FetchContent)

FetchContent_Declare(avm-gen
  GIT_REPOSITORY    https://github.com/avm-energo/avm-gen.git
  GIT_TAG           main
)

FetchContent_MakeAvailable(avm-gen)
