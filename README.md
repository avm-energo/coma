| OS | Status | Download |
|----------|--------|----------|
| Windows x64 | [![](http://172.16.31.73:27015/app/rest/builds/buildType:AVMService_CiAVMServiceWindowsBuild_BuildX64/statusIcon.svg)](http://172.16.31.73:27015/buildConfiguration/AVMService_CiAVMServiceWindowsBuild_BuildX64) | [Download](file://///Fserver/av-tuk/SOFT/AVTUK-S/CI/) |
| Windows x86 | [![](http://172.16.31.73:27015/app/rest/builds/buildType:AVMService_CiAVMServiceWindowsBuild_BuildX86/statusIcon.svg)](http://172.16.31.73:27015/buildConfiguration/AVMService_CiAVMServiceWindowsBuild_BuildX86) | [Download](file://///Fserver/av-tuk/SOFT/AVTUK-S/CI/) |



# Requires:  

- Python - https://www.python.org/ (for pip)
- Python-pip - https://pypi.org/project/pip/ (for conan)
- conan - https://conan.io/downloads.html (cab be installed with python-pip)
- Qt 5.15.2+ (Qt 5.14.2 does not fully supported) - https://www.qt.io/ 
- C++ compiler with full C++17 support (for example MSVC 2019 [compiler] minimum supported MSVC)

C++ language feature support you can check here

MSVC - https://docs.microsoft.com/en-us/cpp/overview/visual-cpp-language-conformance?view=msvc-160

Others - https://en.cppreference.com/w/cpp/compiler_support

* boost/header_only - conan
* LimeReport - submodule
  * JOM - build booster for LimeReport qmake - conan
* QXlsx - submodule
* hidapi - conan if Windows, system package if Linux
* ctti - inside project tree because project was abandoned
* qcustomplot - same as ctti 

# Build: 
How to initialize conan, just run command in repository root folder


```
$ mkdir build && cd build
$ cmake [<options>] ../src 
```


supported options:
* CONAN_EXEC=path_to_conan - neccessary if conan is not in PATH
* USER_DIRECTORY=user_directory - neccessary if you want to use non default conan directory, provide path contains .conan folder
* QT_QMAKE_EXECUTABLE:STRING=path_to_qmake - neccessary if qmake not in PATH
* CMAKE_INSTALL_PREFIX=install_prefix - prefix for cmake --build --target install
* CMAKE_PREFIX_PATH:STRING=path_to_qt - path to Qt location, for example C:\Qt\5.15.2\msvc2019
* A x64/Win32 - used to change target arch only with Visual Studio cmake generator, ninja uses only provided compilers

```$ cmake --build . [<options>]```

supported options for multi-config generators:
* config Release/Debug

```$ cmake --install . [<options>]```

supported options for multi-config generators same as for build step

