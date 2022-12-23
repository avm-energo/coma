include(FetchContent)

FetchContent_Declare(
  QXlsx
  GIT_REPOSITORY https://github.com/QtExcel/QXlsx.git
  GIT_TAG        c90929a4e926fdadfe5e27060821e06b4a2f9a1c
  SOURCE_SUBDIR  QXlsx
)
# Building QXlsx as dynamic library
set(BUILD_SHARED_LIBS ON)
FetchContent_MakeAvailable(QXlsx)
