include(FetchContent)

FetchContent_Declare(avm-widgets
  GIT_REPOSITORY    https://github.com/avm-energo/avm-widgets.git
  GIT_TAG           develop
)

FetchContent_MakeAvailable(avm-widgets)
