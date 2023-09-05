include(FetchContent)

FetchContent_Declare(
  limereport-qt${QT_VERSION_MAJOR}
  GIT_REPOSITORY https://github.com/fralx/LimeReport.git
  GIT_TAG        6a623a9ecbf8083fb01ae08f00aaa81f70720d03  # v1.7.4
#  GIT_TAG        b1c67651c8d90ee822893990574eb112ec76d575
)
# If we want use Qt6
if (${QT_VERSION_MAJOR} EQUAL 6)
  set(USE_QT6 ON)
endif()
FetchContent_MakeAvailable(limereport-qt${QT_VERSION_MAJOR})

add_library(limereport ALIAS limereport-qt${QT_VERSION_MAJOR})

# Create install component for deb packages
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  install(
    TARGETS limereport-qt${QT_VERSION_MAJOR}
    LIBRARY       DESTINATION "${CPACK_PACKAGING_INSTALL_PREFIX}/lib"     COMPONENT LIMEREPORT
  )
endif()
