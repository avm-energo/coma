include(FetchContent)

FetchContent_Declare(qxlsx
  GIT_REPOSITORY    https://github.com/avm-energo/qxlsx.git
  GIT_TAG           v2.0.1
  SOURCE_SUBDIR     QXlsx
)

FetchContent_MakeAvailable(qxlsx)
