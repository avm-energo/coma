include(FetchContent)

FetchContent_Declare(qxlsx
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/libavm-qxlsx.git
  GIT_TAG           main
  SOURCE_SUBDIR     QXlsx
)

FetchContent_MakeAvailable(qxlsx)
