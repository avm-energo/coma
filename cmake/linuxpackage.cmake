# Linux package layout for an application (avm-service, avm-debug), included from its
# CMakeLists.txt after add_executable(). The package itself is assembled outside CMake
# (dpkg-deb / rpmbuild -bb) from a staged install tree:
#
#   cmake --install <build> --component <app> --prefix /usr    # with DESTDIR=<staging>
#
# Everything the package needs is installed under COMPONENT ${PROJECT_NAME}. Install rules
# of the FetchContent dependencies (headers, cmake configs) stay in their own components and
# don't end up in the package.
#
# Resulting tree (prefix /usr):
#   bin/<app>
#   lib[/<multiarch>]/<app>/*.so.*            in-house shared libs, found via RPATH
#   share/<app>/{settings,reports}/          copied to ~/.config on first run (ComaHelper)
#   share/applications/ru.avmenergo.<app>.desktop
#   share/icons/hicolor/48x48/apps/<app>.png
#   lib/udev/rules.d/70-<app>.rules
#
# The in-house libraries (libavm-*, limereport, qxlsx, ...) are shipped in a private
# directory instead of separate packages, so avm-service and avm-debug never conflict and
# never need matching libavm-* package versions. Qt and other system libraries come from
# the distribution (dpkg-shlibdeps / rpm's automatic requires).

include(GNUInstallDirs)

# Collects the non-imported shared libraries `target` links to, transitively.
# Imported targets (Qt, system packages) are skipped - they come from the distribution.
function(avm_collect_private_shared_libs target out_var)
  set(queue ${target})
  set(seen "")
  set(result "")
  while(queue)
    list(POP_FRONT queue current)
    if(NOT TARGET "${current}" OR current IN_LIST seen)
      continue()
    endif()
    list(APPEND seen ${current})
    get_target_property(aliased ${current} ALIASED_TARGET)
    if(aliased)
      list(APPEND queue ${aliased})
      continue()
    endif()
    get_target_property(imported ${current} IMPORTED)
    if(imported)
      continue()
    endif()

    get_target_property(type ${current} TYPE)
    if(type STREQUAL "SHARED_LIBRARY")
      list(APPEND result ${current})
    endif()

    set(links "")
    if(NOT type STREQUAL "INTERFACE_LIBRARY")
      get_target_property(direct ${current} LINK_LIBRARIES)
      if(direct)
        list(APPEND links ${direct})
      endif()
    endif()
    get_target_property(iface ${current} INTERFACE_LINK_LIBRARIES)
    if(iface)
      list(APPEND links ${iface})
    endif()
    foreach(lib IN LISTS links)
      if(lib MATCHES "^\\$<(LINK_ONLY|BUILD_INTERFACE|BUILD_LOCAL_INTERFACE):([^>]+)>$")
        set(lib ${CMAKE_MATCH_2})
      endif()
      # Other generator expressions and CMake's "::@(dir)" scope markers
      if(lib MATCHES "^\\$<" OR lib MATCHES "^::@")
        continue()
      endif()
      list(APPEND queue ${lib})
    endforeach()
  endwhile()
  set(${out_var} ${result} PARENT_SCOPE)
endfunction()

set(AVM_PRIVATE_LIBDIR "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}")
set(AVM_PACKAGE_DATADIR "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")
set(AVM_PACKAGE_SRC "${CMAKE_SOURCE_DIR}/package/src")

# The build tree keeps CMake's absolute build RPATH; installed copies get relative ones.
set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../${AVM_PRIVATE_LIBDIR}")

avm_collect_private_shared_libs(${PROJECT_NAME} AVM_PRIVATE_LIBS)
message(STATUS "${PROJECT_NAME}: private shared libs for the package: ${AVM_PRIVATE_LIBS}")
foreach(lib IN LISTS AVM_PRIVATE_LIBS)
  # Libraries load each other from the same directory. "$ORIGIN" is the same value for
  # every application, so installing a library for both avm-service and avm-debug is fine.
  set_target_properties(${lib} PROPERTIES INSTALL_RPATH "$ORIGIN")
  # Headers of libraries that declare PUBLIC_HEADER are parked in a component that is
  # never installed - without a destination CMake warns, and the package doesn't need them.
  install(TARGETS ${lib}
    LIBRARY DESTINATION ${AVM_PRIVATE_LIBDIR} NAMELINK_SKIP COMPONENT ${PROJECT_NAME}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${PROJECT_NAME}-unused EXCLUDE_FROM_ALL)
endforeach()

# Module XML descriptors and report templates (the Windows installer takes the same set)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/settings/"
  DESTINATION "${AVM_PACKAGE_DATADIR}/settings"
  COMPONENT ${PROJECT_NAME}
  FILES_MATCHING PATTERN "*.xml")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/reports/"
  DESTINATION "${AVM_PACKAGE_DATADIR}/reports"
  COMPONENT ${PROJECT_NAME}
  FILES_MATCHING PATTERN "*.lrxml")

# Desktop integration
install(FILES "${AVM_PACKAGE_SRC}/${PROJECT_NAME}/ru.avmenergo.${PROJECT_NAME}.desktop"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
  COMPONENT ${PROJECT_NAME})
install(FILES "${AVM_PACKAGE_SRC}/${PROJECT_NAME}.png"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps"
  COMPONENT ${PROJECT_NAME})

# USB HID and serial port access for the logged-in user. Always /usr/lib/udev, never lib64/multiarch.
install(FILES "${AVM_PACKAGE_SRC}/udev/avm-devices.rules"
  DESTINATION "lib/udev/rules.d"
  RENAME "70-${PROJECT_NAME}.rules"
  COMPONENT ${PROJECT_NAME})

# RPM spec for `rpmbuild -bb` over the staged tree, see the header of the template
configure_file("${AVM_PACKAGE_SRC}/rpm/app.spec.in"
  "${CMAKE_BINARY_DIR}/rpm/${PROJECT_NAME}.spec"
  @ONLY)
