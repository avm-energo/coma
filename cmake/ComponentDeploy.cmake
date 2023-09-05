# Adding targets to cpack component
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "${CPACK_PACKAGING_INSTALL_PREFIX}/bin" COMPONENT ${COMPONENT_NAME})

# Adding XML files to cpack component
install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../settings"
  DESTINATION "${CPACK_PACKAGING_INSTALL_PREFIX}/share/coma"
  COMPONENT ${COMPONENT_NAME})

# Generating postinst and postrm files
set(POSTINST_IN  "${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/postinst.in")
set(POSTINST_OUT "${CMAKE_CURRENT_BINARY_DIR}/postinst")
set(POSTRM_IN    "${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/postrm.in")
set(POSTRM_OUT   "${CMAKE_CURRENT_BINARY_DIR}/postrm")
configure_file(${POSTINST_IN} ${POSTINST_OUT} @ONLY)
configure_file(${POSTRM_IN}   ${POSTRM_OUT}   @ONLY)
