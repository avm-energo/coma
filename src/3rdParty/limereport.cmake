include(FetchContent)

#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

FetchContent_Declare(
  limereport-qt5
  GIT_REPOSITORY https://github.com/Jihadist/LimeReport.git
  GIT_TAG        22120942129633696b99422fed6b43424e1d3e94
)
# If we want compile lib in static
#set(LIMEREPORT_STATIC ON)
FetchContent_MakeAvailable(limereport-qt5)

add_library(limereport ALIAS limereport-qt5)
#target_include_directories(limereport-qt5 INTERFACE ${limereport_SOURCE_DIR}/include)
