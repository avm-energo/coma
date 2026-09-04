include(FetchContent)

FetchContent_Declare(libavm-emon
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/libavm-emon.git
  GIT_TAG           master
)

FetchContent_MakeAvailable(libavm-emon)
