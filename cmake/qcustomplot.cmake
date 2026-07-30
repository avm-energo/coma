include(FetchContent)

FetchContent_Declare(qcustomplot
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/libavm-qcustomplot.git
  GIT_TAG           main
)

FetchContent_MakeAvailable(qcustomplot)
