include(FetchContent)

FetchContent_Declare(hidapi
  GIT_REPOSITORY    https://github.com/avm-energo/hidapi.git
  GIT_TAG           v1.0.0
  # SOURCE_SUBDIR     windows
)

FetchContent_MakeAvailable(hidapi)
