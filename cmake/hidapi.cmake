include(FetchContent)

FetchContent_Declare(libavm-hidapi
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/libavm-hidapi.git
  GIT_TAG           main
  # SOURCE_SUBDIR     windows
)

FetchContent_MakeAvailable(libavm-hidapi)
