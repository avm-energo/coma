# Package settings
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_PACKAGE_CONTACT "info@avmenergo.ru") # TODO: change to support email?
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${SOFTDEVELOPER} <${CPACK_PACKAGE_CONTACT}>") #required
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "COMplex for Avtuk")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_PACKAGE_RELEASE 1)

# Strip binary files if we have release build
#if(CMAKE_BUILD_TYPE STREQUAL "Release")
#  set(CPACK_STRIP_FILES YES)
#  message(STATUS "Strip binary files for release package")
#endif()

# Debug purposes
message(STATUS "Components to pack: ${CPACK_COMPONENTS_ALL}")

include(CPack)

# Add a component for AVM-Service
cpack_add_component(AVM-Service
    DISPLAY_NAME  "AVM-Service"
    DESCRIPTION   "COMplex for Avtuk"
    GROUP AVM_Service)
# Add a group for AVM-Service
cpack_add_component_group(AVM_Service)

# Add a component for AVM-Debug
cpack_add_component(AVM-Debug
    DISPLAY_NAME  "AVM-Debug"
    DESCRIPTION   "COMplex for Avtuk"
    GROUP AVM_Debug)
# Add a group for AVM_Debug
cpack_add_component_group(AVM_Debug)

