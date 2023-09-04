# Global cpack settings
set(CPACK_GENERATOR "DEB")
set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)
set(CPACK_DEB_COMPONENT_INSTALL ON) # magic required for separate packaging components
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON) # enable inter-component dependencies
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_PACKAGE_CONTACT "info@avmenergo.ru") # TODO: change to support email?
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${SOFTDEVELOPER} <${CPACK_PACKAGE_CONTACT}>") #required
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# Settings for AVM-Debug component
set(CPACK_DEBIAN_AVMDEBUG_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_AVMDEBUG_PACKAGE_NAME "avm-debug")
set(CPACK_DEBIAN_AVMDEBUG_DESCRIPTION  "АВМ-Наладка")
set(DEBUG_POSTINST "${CMAKE_CURRENT_BINARY_DIR}/AVM-Debug/postinst")
set(DEBUG_POSTRM   "${CMAKE_CURRENT_BINARY_DIR}/AVM-Debug/postrm"  )
set(CPACK_DEBIAN_AVMDEBUG_PACKAGE_CONTROL_EXTRA "${DEBUG_POSTINST}" "${DEBUG_POSTRM}")
get_target_property(AVMDEBUG_VERSION AVM-Debug VERSION)
#message(STATUS "AVM-Debug version: ${AVMDEBUG_VERSION}")
set(CPACK_DEBIAN_AVMDEBUG_PACKAGE_VERSION ${AVMDEBUG_VERSION})

# Settings for AVM-Service component
set(CPACK_DEBIAN_AVMSERVICE_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_AVMSERVICE_PACKAGE_NAME "avm-service")
set(CPACK_DEBIAN_AVMSERVICE_DESCRIPTION  "АВМ-Сервис")
set(SERVICE_POSTINST "${CMAKE_CURRENT_BINARY_DIR}/AVM-Service/postinst")
set(SERVICE_POSTRM   "${CMAKE_CURRENT_BINARY_DIR}/AVM-Service/postrm"  )
set(CPACK_DEBIAN_AVMSERVICE_PACKAGE_CONTROL_EXTRA "${SERVICE_POSTINST}" "${SERVICE_POSTRM}")
get_target_property(AVMSERVICE_VERSION AVM-Service VERSION)
#message(STATUS "AVM-Service version: ${AVMSERVICE_VERSION}")
set(CPACK_DEBIAN_AVMSERVICE_PACKAGE_VERSION ${AVMSERVICE_VERSION})

# Settings for gen library component
set(CPACK_DEBIAN_GEN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_GEN_PACKAGE_NAME "gen")
set(CPACK_DEBIAN_GEN_DESCRIPTION  "General library for AVM components")
get_target_property(GEN_VERSION gen VERSION)
set(CPACK_DEBIAN_GEN_PACKAGE_VERSION ${GEN_VERSION})

# Settings for comaresources library component
set(CPACK_DEBIAN_AVMRESOURCES_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_AVMRESOURCES_PACKAGE_NAME "avm-resources")
set(CPACK_DEBIAN_AVMRESOURCES_DESCRIPTION  "АВМ-Ресурсы")
get_target_property(AVMRES_VERSION comaresources VERSION)
set(CPACK_DEBIAN_AVMRESOURCES_PACKAGE_VERSION ${AVMRES_VERSION})

# Settings for qcustomplot library component
set(CPACK_DEBIAN_QCUSTOMPLOT_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_QCUSTOMPLOT_PACKAGE_NAME "qcustomplot")
set(CPACK_DEBIAN_QCUSTOMPLOT_DESCRIPTION  "QCustomPlot")
get_target_property(QCUSTOMPLOT_VERSION qcustomplot VERSION)
#message(STATUS "qcustomplot library version: ${QCUSTOMPLOT_VERSION}")
set(CPACK_DEBIAN_QCUSTOMPLOT_PACKAGE_VERSION ${QCUSTOMPLOT_VERSION})

# Settings for limereport library component
set(CPACK_DEBIAN_LIMEREPORT_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_LIMEREPORT_PACKAGE_NAME "limereport-qt${QT_VERSION_MAJOR}")
set(CPACK_DEBIAN_LIMEREPORT_DESCRIPTION  "Limereport")
set(CPACK_DEBIAN_LIMEREPORT_PACKAGE_VERSION 1.6.8) # TODO: manually version input?

# Settings for QXlsx library component
set(CPACK_DEBIAN_QXLSX_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_QXLSX_PACKAGE_NAME "qxlsx-qt${QT_VERSION_MAJOR}")
set(CPACK_DEBIAN_QXLSX_DESCRIPTION  "QXlsx")
get_target_property(QXLSX_VERSION QXlsx VERSION)
#message(STATUS "QXlsx library version: ${QXLSX_VERSION}")
set(CPACK_DEBIAN_QXLSX_PACKAGE_VERSION ${QXLSX_VERSION})

# Package component dependencies
set(PACKAGE_DEPENDENCIES GEN AVMRESOURCES QCUSTOMPLOT LIMEREPORT QXLSX)
set(PACKAGING_PATH_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/_CPack_Packages/${CMAKE_SYSTEM_NAME}/${CPACK_GENERATOR}/${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")
set(PACKAGING_PATH_SUFFIX "usr/local/lib")
foreach(_depend_var IN LISTS PACKAGE_DEPENDENCIES)
  list(APPEND CPACK_DEBIAN_PACKAGE_SHLIBDEPS_PRIVATE_DIRS "${PACKAGING_PATH_PREFIX}/${_depend_var}/${PACKAGING_PATH_SUFFIX}")
endforeach()
message(STATUS "${CPACK_DEBIAN_PACKAGE_SHLIBDEPS_PRIVATE_DIRS}")
set(CPACK_COMPONENT_AVMDEBUG_DEPENDS   ${PACKAGE_DEPENDENCIES})
set(CPACK_COMPONENT_AVMSERVICE_DEPENDS ${PACKAGE_DEPENDENCIES})

# Debug purposes
set(CPACK_DEBIAN_PACKAGE_DEBUG ON)

include(CPack)
