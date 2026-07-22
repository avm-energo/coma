include(FetchContent)

FetchContent_Declare(magic-enum
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/libavm-magic_enum.git
  GIT_TAG           master
)

FetchContent_MakeAvailable(magic-enum)
