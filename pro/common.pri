CONFIG(release, debug|release):BUILDDIR  = build-$${TARGET}-$${QT_VERSION}

win32 {
    win32-msvc* {
    CONFIG(release, debug|release):BUILDDIR =$${BUILDDIR}-$${QMAKE_COMPILER}-$${MSVC_VER}-$${QMAKE_TARGET.arch}
    }
}

# QMAKE_COMPILER  = gcc clang llvm   # clang pretends to be gcc
clang {
    COMPILER_VERSION = $$system($$QMAKE_CXX " -dumpversion")
    SPLITTED_COMPILER_VERSION=$$split(COMPILER_VERSION, .)
    COMPILER_MAJOR_VERSION = $$first(SPLITTED_COMPILER_VERSION)
    CONFIG(release, debug|release):BUILDDIR =$${BUILDDIR}-clang-$${COMPILER_MAJOR_VERSION}-$${QT_ARCH}
}
else {
    # QMAKE_COMPILER = gcc llvm
    llvm {
        COMPILER_VERSION = $$system($$QMAKE_CXX " -dumpversion")
        SPLITTED_COMPILER_VERSION=$$split(COMPILER_VERSION, .)
        COMPILER_MAJOR_VERSION = $$first(SPLITTED_COMPILER_VERSION)
        CONFIG(release, debug|release):BUILDDIR =$${BUILDDIR}-llvm-$${COMPILER_MAJOR_VERSION}-$${QT_ARCH}
    }
    else {
    # QMAKE_COMPILER = gcc
        gcc {
            COMPILER_VERSION = $$system($$QMAKE_CXX " -dumpversion")
            SPLITTED_COMPILER_VERSION=$$split(COMPILER_VERSION, .)
            COMPILER_MAJOR_VERSION = $$first(SPLITTED_COMPILER_VERSION)
            CONFIG(release, debug|release):BUILDDIR =$${BUILDDIR}-gcc-$${COMPILER_MAJOR_VERSION}-$${QT_ARCH}
        }
    }
}

CONFIG(release, debug|release):message($${BUILDDIR})
CONFIG(release, debug|release):OBJECTS_DIR = $${BUILDDIR}/.obj
CONFIG(release, debug|release):MOC_DIR = $${BUILDDIR}/.moc
CONFIG(release, debug|release):RCC_DIR = $${BUILDDIR}/.rcc
CONFIG(release, debug|release):UI_DIR = $${BUILDDIR}/.ui

# copies the given files to the destination directory
defineTest(copyToDestDir) {
    files = $$1
    dir = $$2
    # replace slashes in destination path for Windows
    win32:dir ~= s,/,\\,g

    for(file, files) {
        # replace slashes in source path for Windows
        win32:file ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_quote($$file) $$shell_quote($$dir) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}
