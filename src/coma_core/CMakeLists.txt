set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/coma_core")
set(SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(HEADERS_H
  "${INCLUDE_PATH}/coma.h"
)

set(SOURCES_CPP
  "${SRC_PATH}/coma.cpp"
)

# Creating library target
add_library(coma_core STATIC ${HEADERS_H} ${SOURCES_CPP})

# Including directories for library
target_include_directories(coma_core PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# Linking libraries for current target
target_link_libraries(coma_core PUBLIC dialogs comaresources comaversion)
