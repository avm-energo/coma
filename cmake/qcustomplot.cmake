include(FetchContent)

FetchContent_Declare(
  qcustomplot
  GIT_REPOSITORY https://github.com/avm-energo/qcustomplot.git
  GIT_TAG        origin/develop
)
FetchContent_MakeAvailable(qcustomplot)
