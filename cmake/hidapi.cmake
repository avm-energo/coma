include(FetchContent)

FetchContent_Declare(hidapi
  GIT_REPOSITORY    https://github.com/avm-energo/hidapi.git
  GIT_TAG           main
  # SOURCE_SUBDIR     windows
)

FetchContent_MakeAvailable(hidapi)
