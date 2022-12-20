cmake_minimum_required(VERSION 3.15)
include(FetchContent)

FetchContent_Declare(
  limereport-qt5
  GIT_REPOSITORY https://github.com/fralx/LimeReport.git
  GIT_TAG        b1c67651c8d90ee822893990574eb112ec76d575
)
# If we want compile lib in static
#set(LIMEREPORT_STATIC ON)
FetchContent_MakeAvailable(limereport-qt5)

add_library(limereport ALIAS limereport-qt5)
