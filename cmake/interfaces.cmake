include(FetchContent)

FetchContent_Declare(libavm-interfaces
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/libavm-interfaces.git
  GIT_TAG           BlockDataBlock
)

FetchContent_MakeAvailable(libavm-interfaces)
