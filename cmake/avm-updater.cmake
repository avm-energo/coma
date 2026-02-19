include(FetchContent)

FetchContent_Declare(avm-updater
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/avm-updater.git
  GIT_TAG           main
)

FetchContent_MakeAvailable(avm-updater)
