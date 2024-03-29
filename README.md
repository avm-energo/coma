| OS | Status | Download |
|----------|--------|----------|
| Windows x64 | [![](http://172.16.31.73:27015/app/rest/builds/buildType:Coma_WindowsBuild_Develop_BuildX64/statusIcon.svg)](http://172.16.31.73:27015/buildConfiguration/Coma_WindowsBuild_Develop_BuildX64) | [Download](file://///Rserver/av-tuk/SOFT/AVTUK-S/) |
| Windows x86 | [![](http://172.16.31.73:27015/app/rest/builds/buildType:Coma_WindowsBuild_Develop_BuildX86/statusIcon.svg)](http://172.16.31.73:27015/buildConfiguration/Coma_WindowsBuild_Develop_BuildX86) | [Download](file://///Rserver/av-tuk/SOFT/AVTUK-S/) |
| Linux (Debian 10) x64 | [![](http://172.16.31.73:27015/app/rest/builds/buildType:Coma_LinuxBuild_Develop_BuildX64/statusIcon.svg)](http://172.16.31.73:27015/buildConfiguration/Coma_LinuxBuild_Develop_BuildX64) | [Download](file://///Rserver/av-tuk/SOFT/AVTUK-S/) |
| Linux (Debian 10) armv7 | [![](http://172.16.31.73:27015/app/rest/builds/buildType:Coma_LinuxBuild_Develop_BuildArmv7a/statusIcon.svg)](http://172.16.31.73:27015/admin/editBuildRunners.html?id=buildType:Coma_LinuxBuild_AliseZero_BuildArmv7a) | [Download](file://///Rserver/av-tuk/SOFT/AVTUK-S/) |

# Requires

- Tools:
  - C++ compiler with full C++17 support (MSVC 2019 - 16.10+, gcc 9.3.0+, clang-11+)
  - CMake and Ninja for cross-platform project generating
  - Python - https://www.python.org/ (with pip, for windows 7 install python 3.8.10 or older)
  - conan - https://conan.io/downloads.html (can be installed with **pip install conan**, ensure that the dir with python/scripts is in the $PATH)

- C++ language feature support you can check here:
  - MSVC - https://docs.microsoft.com/en-us/cpp/overview/visual-cpp-language-conformance?view=msvc-160
  - Others - https://en.cppreference.com/w/cpp/compiler_support

- Dependencies:
  - Qt 5.15.2 fully supported, a minimal functional can be compiled with Qt 5.11.1 - https://www.qt.io/ 
  - boost/header_only - installs by conan automatically
  - LimeReport - installs from GitHub repo by CMake, building as sub-target
  - QXlsx - installs from GitHub repo by CMake, building as sub-target
  - hidapi - installs by conan automatically, using system package under Linux
  - ctti - inside project tree because project was abandoned
  - qcustomplot - installs from GitHub repo by CMake, building as sub-target
  - protobuf - installs by conan automatically
  - zeromq - installs by conan automatically

# Build 

conan: if there's an error: settings:compiler not set, execute the following commands (for VS2019):

**conan profile update "settings.compiler=Visual Studio" default
conan profile update "settings.compiler.version=16" default**

```bash
$ mkdir build && cd build
$ cmake [<options>] ../src 
```

supported options:
- cmake  
  - CMAKE_TOOLCHAIN_FILE - custom toolchain file if needed
  - CMAKE_BUILD_TYPE - build type of course
  - CMAKE_INSTALL_PREFIX=install_prefix - prefix for cmake --build --target install
  - CMAKE_PREFIX_PATH:STRING=path_to_qt - path to Qt location, for example C:\Qt\5.15.2\msvc2019
- git 
  - COMMIT_COUNT - output of command ```git rev-list --count HEAD```, neccessary if you don't have git in PATH or you don't have git locally
  - SHORT_HASH - output of command ```git rev-parse --short"="8 HEAD```, neccessary if you don't have git in PATH or you don't have git locally
- cpack
  - CPACK_PACKAGING_INSTALL_PREFIX=/usr/local - prefix for installed files (now ony for deb packages)
- other 
  - A x64/Win32 - used to change target arch only with Visual Studio cmake generator, ninja uses only provided compilers
  - BUILD_WITH_ALISE:BOOL=ON (off by default) - do u wanna build Alise? Use only if you know her.
  - ENABLE_EMULATOR:BOOL=ON (off by default) - coma'll be built with dummy interface, it can be used without real device 

For the build of ALiSe run: conan profile update settings.compiler.libcxx=libstdc++11 default

  ```$ cmake --build . [<options>]```

supported options for multi-config generators:
- config Release/Debug

```$ cmake --install . [<options>]```

supported options for multi-config generators same as for build step


#### Supported architectures
|       | x86  | x86_64 | armv7    |
| ----- | ---- | ------ | -------- |
| core  | +    | +      | +        |
| gui   | +    | +      | -        |
| avtuk | +    | +      | only CCU |

Non gui parts can be compiled for x86 (i386, win32), x86_64 (amd64, win64), armv7(armv7a, armhf)
