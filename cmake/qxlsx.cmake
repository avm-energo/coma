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

# Create install component for deb packages
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  install(
    TARGETS QXlsx
    LIBRARY DESTINATION       "/usr/local/lib"     COMPONENT QXLSX
    ARCHIVE DESTINATION       "/usr/local/lib"     COMPONENT QXLSX
#    PUBLIC_HEADER DESTINATION "/usr/local/include" COMPONENT QXLSX
  )
endif()
