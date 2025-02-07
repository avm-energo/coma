include(FetchContent)

FetchContent_Declare(avm-gen
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/gen.git
  GIT_TAG           v1.9.1
)

FetchContent_MakeAvailable(avm-gen)
