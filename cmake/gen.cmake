include(FetchContent)

FetchContent_Declare(libavm-gen
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/libavm-gen.git
  GIT_TAG           setConfig
)

FetchContent_MakeAvailable(libavm-gen)
