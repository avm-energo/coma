include(FetchContent)

FetchContent_Declare(avm-gen
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/avm-gen.git
  GIT_TAG           v1.10.0
)

FetchContent_MakeAvailable(avm-gen)
