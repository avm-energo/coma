include(FetchContent)

FetchContent_Declare(
  limereport-qt5
  GIT_REPOSITORY https://github.com/fralx/LimeReport.git
  GIT_TAG        0944aa15bd857e0888c0fb3aae823aafedacb5be
)
# If we want compile lib in static
set(LIMEREPORT_STATIC ON)
FetchContent_MakeAvailable(limereport-qt5)

add_library(limereport ALIAS limereport-qt5)
target_include_directories(limereport-qt5 INTERFACE ${limereport_SOURCE_DIR}/include)
