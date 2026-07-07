include(FetchContent)

FetchContent_Declare(libavm-widgets
  GIT_REPOSITORY    ssh://git@git.avmenergo.ru:5507/avm-energo/libavm-widgets.git
  GIT_TAG           elabel-copyable
)

FetchContent_MakeAvailable(libavm-widgets)
