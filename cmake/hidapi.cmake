include(FetchContent)

FetchContent_Declare(hidapi
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/hidapi.git
  GIT_TAG           main
  # SOURCE_SUBDIR     windows
)

FetchContent_MakeAvailable(hidapi)
