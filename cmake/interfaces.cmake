include(FetchContent)

FetchContent_Declare(libavm-interfaces
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/libavm-interfaces.git
  GIT_TAG           AddMTCP
)

FetchContent_MakeAvailable(libavm-interfaces)
