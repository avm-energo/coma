PROJECT_ROOT_PATH = $${PWD}/

CONFIG += c++17

QMAKE_TARGET_COMPANY = AVM-Energo
QMAKE_TARGET_COPYRIGHT = AVM-Energo
QMAKE_TARGET_PRODUCT = AVTUK

CONFIG(debug, debug|release) {
    BUILD_FLAG = debug
    LIB_SUFFIX = d
} else {
    BUILD_FLAG = release
}

win32 {
    LIBFORMAT = .lib
} else {
    LIBPREFIX = lib
    LIBFORMAT = .a
}

LIBS_PATH = $${PROJECT_ROOT_PATH}/lib/$${QT_ARCH}/$${BUILD_FLAG}/
INC_PATH = $${PROJECT_ROOT_PATH}/include/
IMPORT_PATH = $${PROJECT_ROOT_PATH}/
BIN_PATH = $${PROJECT_ROOT_PATH}/../bin/$${QT_ARCH}/$${BUILD_FLAG}/

BUILDDIR  = build-$${QT_VERSION}
BINDIR =
win32 {
    win32-msvc* {
    BUILDDIR =$${BUILDDIR}-$${QMAKE_COMPILER}-$${MSVC_VER}-$${QMAKE_TARGET.arch}
    }
}

# QMAKE_COMPILER  = gcc clang llvm   # clang pretends to be gcc
clang {
    COMPILER_VERSION = $$system($$QMAKE_CXX " -dumpversion")
    SPLITTED_COMPILER_VERSION=$$split(COMPILER_VERSION, .)
    COMPILER_MAJOR_VERSION = $$first(SPLITTED_COMPILER_VERSION)
    BUILDDIR =$${BUILDDIR}-clang-$${COMPILER_MAJOR_VERSION}-$${QT_ARCH}
}
else {
    # QMAKE_COMPILER = gcc llvm
    llvm {
        COMPILER_VERSION = $$system($$QMAKE_CXX " -dumpversion")
        SPLITTED_COMPILER_VERSION=$$split(COMPILER_VERSION, .)
        COMPILER_MAJOR_VERSION = $$first(SPLITTED_COMPILER_VERSION)
        BUILDDIR =$${BUILDDIR}-llvm-$${COMPILER_MAJOR_VERSION}-$${QT_ARCH}
    }
    else {
    # QMAKE_COMPILER = gcc
        gcc {
            COMPILER_VERSION = $$system($$QMAKE_CXX " -dumpversion")
            SPLITTED_COMPILER_VERSION=$$split(COMPILER_VERSION, .)
            COMPILER_MAJOR_VERSION = $$first(SPLITTED_COMPILER_VERSION)
            BUILDDIR =$${BUILDDIR}-gcc-$${COMPILER_MAJOR_VERSION}-$${QT_ARCH}
        }
    }
}

BUILD_PATH = $${PROJECT_ROOT_PATH}/build/$${BUILD_FLAG}/$${TARGET}/
RCC_DIR = $${BUILD_PATH}/.rcc/
UI_DIR = $${BUILD_PATH}/.ui/
MOC_DIR = $${BUILD_PATH}/.moc/
OBJECTS_DIR = $${BUILD_PATH}/.obj/

LIBS += -L$${LIBS_PATH}/
INCLUDEPATH += $${INC_PATH}/
INCLUDEPATH += ../$${IMPORT_PATH}/

DEFINES += DEVICETYPE=1 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

QXLSX_PARENTPATH=./../include/QXlsx/QXlsx         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./../include/QXlsx/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./../include/QXlsx/QXlsx/source/  # current QXlsx source path is ./source/
LIME_HEADERPATH=./../include/LimeReport


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
