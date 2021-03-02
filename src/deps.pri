!isEmpty(LIBRARIES) {
    for(libpath, LIBRARIES) {
        out_libpath = $${LIBS_PATH}
        libbase = $$basename(libpath)
        LIBS += -l$$libbase$${LIB_SUFFIX}
        PRE_TARGETDEPS+=$${out_libpath}/$${LIBDIR}/$${LIBPREFIX}$${libbase}$${LIB_SUFFIX}$${LIBFORMAT}
    }
}
