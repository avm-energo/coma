include(FetchContent)

FetchContent_Declare(hidapi
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/hidapi.git
  GIT_TAG           v1.0.0
  # SOURCE_SUBDIR     windows
)

FetchContent_MakeAvailable(hidapi)
