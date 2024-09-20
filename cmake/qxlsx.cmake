include(FetchContent)

FetchContent_Declare(qxlsx
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/QXlsx.git
  GIT_TAG           master
  SOURCE_SUBDIR     QXlsx
)

FetchContent_MakeAvailable(qxlsx)
