include(FetchContent)

FetchContent_Declare(
  qcustomplot
  GIT_REPOSITORY ssh://git@git.avmenergo.ru:5507/avm-energo/qcustomplot.git
  GIT_TAG        origin/develop
)
FetchContent_MakeAvailable(qcustomplot)
