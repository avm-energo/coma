# COMA

COMA (COMplex for AV-TUK) is a Qt6/C++17 desktop application by AVM-Energo for communicating with,
configuring, tuning and diagnosing "AV-TUK" automatic-controller modules over serial (RS-485), USB HID
and Ethernet (Modbus RTU, IEC 60870-5-104 and the proprietary Protocom protocol).

Two executables are built from the same codebase:

- **avm-service** - the production/operator build.
- **avm-debug** - the same UI plus the extra debug/tuning menus.

A third one, **avm-autobatch**, is a helper for batch processing (see `doc/abatcher-task.md`).

## Builds

Built by GitLab CI (`.gitlab-ci.yml`) on every push:

| OS | Environment | Job | Status | Download |
|----|-------------|-----|--------|----------|
| Windows x64 | MSVC 2022, Qt 6 (`msvc2022_64`), dedicated runner with the `windows` tag | `build:windows` | [![build-windows](https://git.avmenergo.ru/avm-energo/coma/-/jobs/artifacts/master/raw/badges/build-windows.svg?job=badges%3Astatus)](https://git.avmenergo.ru/avm-energo/coma/-/pipelines?ref=master) | [Releases](https://git.avmenergo.ru/avm-energo/coma/-/releases) |
| Linux x64 | Debian, Qt 6 | `build:linux-debian` | [![build-linux-debian](https://git.avmenergo.ru/avm-energo/coma/-/jobs/artifacts/master/raw/badges/build-linux-debian.svg?job=badges%3Astatus)](https://git.avmenergo.ru/avm-energo/coma/-/pipelines?ref=master) | [Releases](https://git.avmenergo.ru/avm-energo/coma/-/releases) |
| Linux x64 | Fedora, Qt 6 | `build:linux-fedora` | [![build-linux-fedora](https://git.avmenergo.ru/avm-energo/coma/-/jobs/artifacts/master/raw/badges/build-linux-fedora.svg?job=badges%3Astatus)](https://git.avmenergo.ru/avm-energo/coma/-/pipelines?ref=master) | [Releases](https://git.avmenergo.ru/avm-energo/coma/-/releases) |

The latest release is always available at
[releases/permalink/latest](https://git.avmenergo.ru/avm-energo/coma/-/releases/permalink/latest).
Pipelines and job logs: [CI/CD pipelines](https://git.avmenergo.ru/avm-energo/coma/-/pipelines).

## Requirements

- Tools:
  - C++ compiler with full C++17 support (MSVC 2022, gcc 9+, clang 11+)
  - CMake 3.21+ and Ninja (or the Visual Studio generator on Windows)
  - git - all the libraries below are fetched from `git.avmenergo.ru` by CMake `FetchContent` at configure
    time, so the build machine needs network access to that host
- Qt 6 (Core, Gui, Widgets, PrintSupport, Network, SerialPort, Concurrent) - https://www.qt.io/
- No package manager (conan, vcpkg, ...) is used.

## Dependencies

All of them are found as installed CMake packages first, and fetched and built as sub-targets if they are
not found (see `CMakeLists.txt` and `cmake/*.cmake`):

| Library | Version | Purpose |
|---------|---------|---------|
| libavm-gen | 3.0.4 | common utilities: settings, logging, error queue, helpers |
| libavm-widgets | 3.0.2 | common Qt widgets (`EMessageBox`, `EStatusBar`, `WDFunc`, ...) |
| libavm-interfaces | 1.0.0 | transport/protocol layer: Ethernet, serial, USB HID; Modbus, IEC 60870-5-104, Protocom |
| libavm-s2 | 1.0.0 | S2 binary configuration/data format |
| libavm-ctti | 1.0.0 | compile-time type introspection |
| libavm-hidapi | 1.0.2 | USB HID access (pulled in through libavm-interfaces) |
| libavm-limereport | 2.0.0 | report designer/engine |
| libavm-qcustomplot | 2.1.2 | plotting |
| libavm-qxlsx | 3.0.0 | Excel export |
| libavm-better-enums | 0.11.3 | enums with reflection |

## Build

```bash
git clone https://git.avmenergo.ru/avm-energo/coma.git
cd coma
# configure: generate the build system into the "build" directory
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/qt
# compile everything, using all CPU cores
cmake --build build --parallel
```

On Windows, with PowerShell and the Visual Studio generator:

```powershell
cmake -B build -S . -A x64 -DCMAKE_PREFIX_PATH=path\to\qt
cmake --build build --config Release --parallel
```

### Options

- CMake:
  - `CMAKE_BUILD_TYPE` - build type (single-config generators)
  - `CMAKE_PREFIX_PATH` - path to the Qt kit, for example `path\to\qt`
  - `CMAKE_INSTALL_PREFIX` - prefix for `cmake --install`
  - `CMAKE_TOOLCHAIN_FILE` - custom toolchain file if needed
  - `-A x64` / `-A Win32` - target architecture, only with the Visual Studio generator
- Project:
  - `BUILD_SHARED_LIBS` (`ON` by default) - build the libraries as shared
  - `BUILD_TESTING` (`OFF` by default) - build the tests from `src/tests`
- git (only needed if git is not in `PATH` or the sources are not a git checkout):
  - `COMMIT_COUNT` - output of `git rev-list --count HEAD`
  - `SHORT_HASH` - output of `git rev-parse --short=8 HEAD`

### Tests

```bash
cmake -S . -B build/test -G Ninja -DBUILD_TESTING=ON
cmake --build build/test
ctest --test-dir build/test
```

### Code style

Formatting is enforced by `clang-format` (see `.clang-format`): `./clang.sh`, or
`find src -iname "*.h" -o -iname "*.cpp" | xargs clang-format -i --style=file`.

## Module descriptors

Most module-specific behaviour (signals, alarms, config records, protocol mappings, displayed
sections) is data-driven from the XML descriptors in `settings/`. Read `settings/README.md` before adding
a new module type.
