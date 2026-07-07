include(FetchContent)

FetchContent_Declare(libavm-gen
  GIT_REPOSITORY    ssh://git@git.avmenergo.ru:5507/avm-energo/libavm-gen.git
  GIT_TAG           main
)

FetchContent_MakeAvailable(libavm-gen)
