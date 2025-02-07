include(FetchContent)

FetchContent_Declare(avm-widgets
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/avm-widgets.git
  GIT_TAG           develop
)

FetchContent_MakeAvailable(avm-widgets)
