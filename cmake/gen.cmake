include(FetchContent)

FetchContent_Declare(avm-gen
  GIT_REPOSITORY    https://gt.avmenergo.ru/avm-energo/avm-gen.git
  GIT_TAG           v1.8.1
)

FetchContent_MakeAvailable(avm-gen)
