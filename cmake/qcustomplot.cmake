include(FetchContent)

FetchContent_Declare(
  qcustomplot
  GIT_REPOSITORY https://github.com/avm-energo/qcustomplot.git
  GIT_TAG        origin/develop
)
FetchContent_MakeAvailable(qcustomplot)

# Create install component for deb packages
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  install(TARGETS qcustomplot LIBRARY DESTINATION "${CPACK_PACKAGING_INSTALL_PREFIX}/lib" COMPONENT QCUSTOMPLOT)
endif()
