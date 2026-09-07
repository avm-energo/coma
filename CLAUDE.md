# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

COMA (COMplex for AV-TUK) is a Qt6/C++17 desktop application by AVM-Energo for communicating with,
configuring, tuning, and diagnosing "AV-TUK" automatic-controller hardware modules over serial (RS-485),
USB HID, and Ethernet (Modbus RTU, IEC 60870-5-104, and a proprietary "Protocom" protocol). It ships as
two executables built from the same codebase:

- **avm-service** (`src/avm-service`) — production/operator build.
- **avm-debug** (`src/avm-debug`) — the same UI plus extra debug/tuning ("наладка") menus, gated at
  runtime by `AppConfiguration::app() == AppConfiguration::Debug` (see `common/appconfig.h`).

Both are thin `main.cpp` wrappers around the shared `Coma` (`QMainWindow`) class in the `coma` static
library (`src/coma`).

Primary user/UI language is Russian; comments, dialog strings, and doc files are largely in Russian.

## Build

Dependencies: Qt 6 (Core, Gui, Widgets, PrintSupport, Network, SerialPort, Concurrent), CMake + Ninja,
conan (for boost header-only, hidapi, protobuf, zeromq). `libavm-widgets`, `hidapi`, `limereport`,
`qcustomplot`, `qxlsx` are fetched automatically via `FetchContent`/conan if not already found as
installed packages — see `cmake/*.cmake` and `CMakeLists.txt`.

```bash
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/path/to/qt ../src   # or ../ from repo root
cmake --build .
```

Useful cmake options (see README.md for the full list):
- `BUILD_TESTING:BOOL=ON` — enable the test suite (off by default).
- `ENABLE_EMULATOR:BOOL=ON` — build against a dummy device interface, no real hardware needed.
- `CMAKE_PREFIX_PATH` — path to Qt install.
- `BUILD_WITH_ALISE:BOOL=ON` — only relevant if you know what Alise is; leave off otherwise.

This repo has an existing configured build tree at `build/Desktop-Debug` (Ninja, clang++, Debug,
`BUILD_TESTING=OFF`). `.clangd` points at it for compile_commands.json. To build against it:

```bash
cmake --build build/Desktop-Debug
```

To run the test suite you must reconfigure with testing on (the existing `Desktop-Debug` cache has it
off):

```bash
cmake -S . -B build/test -G Ninja -DBUILD_TESTING=ON
cmake --build build/test
ctest --test-dir build/test              # all tests
ctest --test-dir build/test -R interfaces_tests   # single test binary
```

Test targets (`src/tests/`, `src/interfaces/tests/`): `datablock`, `xmlParser`, `files`,
`interfaces_tests` (IEC 60870-5-104 APCI/ASDU, C-block, file transfer; Modbus compatibility). Qt Test
framework is used for `interfaces_tests`; others are plain executables registered via `add_test`.
`src/tests/protocom` and `src/tests/xmlValidator` build standalone helper executables (not wired into
`add_test`).

## Formatting

Style is enforced by `clang-format` (see `.clang-format`: WebKit-based, Allman braces, 120 cols).

- Reformat everything: `./clang.sh` (hardcodes a Windows path to `clang-format`; on Linux/macOS just run
  the `find ... | xargs clang-format -i --style=file` part it wraps)
- Check without modifying: `find src -iname "*.h" -o -iname "*.cpp" | xargs clang-format --dry-run --Werror`

## Static analysis

`clazy` (Qt-aware clang static analyzer) reads the `compile_commands.json` produced by any configured
build dir (`CMAKE_EXPORT_COMPILE_COMMANDS` is `ON`):

```bash
find src/coma/src src/interfaces/src src/avm-debug src/avm-service -name "*.cpp" \
  | xargs -P"$(nproc)" -I{} clazy-standalone -p build/Desktop-Debug --checks=level1 \
      --header-filter="$(pwd)/src" {}
```

See README.md's "Code style and static analysis" section for details (single-file invocation, check
levels, `clang-tidy` usage).

## Architecture

### Module layout

- `src/coma` — the core static library. Nearly all application logic lives here, organized by feature
  under `include/` and `src/` (mirrored trees):
  - `device/` — `CurrentDevice` (the connected device: BSI, BSI Extended, config storage, S2 data
    manager, file provider) and `DeviceFabric`, which builds a `CurrentDevice` from an
    `Interface::AsyncConnection`. `device_list.h` enumerates known base/mezzanine board types and models.
  - `xml/` — loads and parses the per-module-type XML descriptors that drive nearly everything
    module-specific (`xml/xmlparser`), plus a WYSIWYG `xmleditor/` for editing them.
  - `s2/` — "S2" binary config/data format handling (`S2DataManager`, `S2ConfigStorage`,
    `S2DataFactory`) — the wire format used to read/write module configuration blocks.
  - `datablocks/` — per-module data structures, one subfolder per base-board family (`21`, `82`, `85`,
    `kiv`).
  - `tune/` — module tuning/calibration dialogs, also split by board family (`21`, `82`, `84`, `85`,
    `kiv`).
  - `engines/` — background operations driving reads/writes/slices against the device.
  - `oscillograms/` — oscillogram (waveform) capture viewing/loading/conversion, including switch-journal
    (`.swj`) files; `dialogs/`, `parsers/`, `viewmodels/` subfolders.
  - `journals/` — event/measurement/system/work journal viewers and parsers (`.jn*`, `.dat` files).
  - `alarms/`, `dialogs/`, `models/`, `comawidgets/`, `common/`, `startup/` — UI widgets, shared dialogs,
    Qt item-models, app config/versioning, and startup/splash sequencing.
- `src/interfaces` — standalone library for the transport/protocol layer: `ifaces/` (Ethernet, serial,
  USB HID transports), `conn/` (`AsyncConnection`/`SyncConnection`), `exec/` (query executors, including
  an IEC104-specific one, via `query_executor_fabric`), `parsers/` (Modbus, IEC104, Protocom
  request/response parsers), `types/` (per-protocol settings structs), `utils/request_queue`. Has its own
  test suite under `tests/` using Qt Test. `coma` depends on `interfaces`.
- `src/ctti` — vendored/in-tree copy of the CTTI (compile-time type introspection) library (upstream
  project is abandoned, kept in-tree per README).
- `src/avm-debug`, `src/avm-service` — the two thin executable entry points described above.
- `settings/` — XML module descriptors (`0021.xml`, `8585.xml`, `bsi.xml`, `s2files.xml`, etc.), one per
  base/mezzanine board combination, plus `module-template.xml` as a starting point and `README.md` /
  `s2files.md` documenting the schema in depth. These are data files consumed by `xml/xmlparser` at
  runtime, not build-time config — read `settings/README.md` before touching module XML structure or
  adding a new module type.

### Key flow: connecting to a device

1. `Coma::connectDialog` → `ConnectDialog` collects `ConnectionSettings`.
2. `Coma::initConnection` → `Interface::ConnectionManager::createConnection` opens the transport
   (`src/interfaces`).
3. `Coma::initDevice` → `Device::DeviceFabric::create` builds a `Device::CurrentDevice` and calls
   `initBSI()` to read the device's Block Startup Info (identifies board/mezzanine type, serial, etc.).
4. On `initBSIFinished`, `Coma::initInterfaceConnection` → `Coma::loadXML` loads the matching module XML
   via `Xml::ConfigLoader` (matched against BSI base/mezzanine type from `settings/*.xml`), then
   `Coma::prepareDialogs` builds the module-specific UI (alarms, dialogs, S2 config request) driven by
   that XML.

Because most module-specific behavior (signals, alarms, config records, protocol mappings, displayed
sections/widgets) is data-driven from `settings/*.xml` rather than hardcoded per-module C++, adding
support for a new module variant is often primarily an XML-authoring task (see `settings/README.md`),
with new C++ needed mainly for genuinely new widget types, protocols, or datablock structures.

### Communication pipeline (`src/interfaces`)

Documented in depth in `doc/coma_2.11.0.pdf` ("COMA Core Architecture Guidelines"); the class names below
still match the current tree even though the doc itself predates some later changes, so treat it as a
reliable map but verify specifics against the code before relying on fine detail.

- `BaseInterface` (→ `SerialPort`, `UsbHidPort`, `Ethernet`) is the abstract transport. Its execution
  thread lives in `BaseInterface::poll`; it emits `dataReceived`/`error`/`reconnected` and exposes
  `reconnect()`, but a reconnect is only *triggered* by the layer above (`ConnectionManager`), not by the
  interface itself.
- `BaseRequestParser`/`BaseResponseParser` (→ per-protocol `Protocom`/`Modbus`/`Iec104` variants) turn
  `CommandStruct` requests into wire bytes and accumulate/validate/parse wire bytes back into
  `DeviceResponse`. Requests or responses that don't fit one frame use `writingLongData`/`readingLongData`
  signals and `getNextDataSection`/`getNextContinueCommand` to stream in chunks (e.g. file transfers).
- `RequestQueue` is a mutex-guarded queue of `CommandStruct`, shared between `AsyncConnection` (producer,
  GUI thread) and `DefaultQueryExecutor` (consumer, its own thread).
- `DefaultQueryExecutor` runs the request/response state machine (`Starting → RequestParsing ⇄ Pending ⇄
  ReadingLongData/WritingLongData → Stopping`, see the PDF's state diagram) against `BaseRequestParser` /
  `BaseResponseParser`, writing to and reading from `BaseInterface`. `Iec104QueryExecutor` subclasses it
  because IEC 60870-5-104 is asynchronous (the device can push unsolicited frames, needs S-frame ACK
  bookkeeping and U-frame/control-block handling) — Protocom (USB/HID) and Modbus (RS-485) are strictly
  synchronous request→response and use the base executor as-is via `query_executor_fabric`.
- `AsyncConnection` is the client-facing façade (`requestData`, `writeData`, `response` signal) that hides
  transport/protocol details; `ConnectionContext::init/run/reset` wires a `BaseInterface` +
  `DefaultQueryExecutor` pair together (each on their own thread) behind one `AsyncConnection`.
  `SyncConnection` is a thin adapter around an `AsyncConnection` that blocks on an event loop
  (`syncRequestData`/`syncWriteData`) — used mainly during tuning ("регулировка") flows where the next
  step depends synchronously on the previous response.
- `ConnectionManager` owns the `ConnectionContext` and the current `AsyncConnection`, and also owns
  reconnect policy: `handleErrors` counts interface/timeout failures and past a threshold emits
  `reconnectInterface` (puts `BaseInterface` into reconnect mode) / `reconnectUI`; once the interface
  reconnects with the old parameters it re-sends a BSI request and, if that succeeds, emits
  `reconnectSuccess` to resume normal operation.

### UI wiring to a connected device

- `ModuleDataUpdater` is the per-widget data-exchange engine: it holds a query list (`BdQuery`) sent over
  `AsyncConnection` via `requestUpdates()`, and forwards responses to `updateData()`.
- `UWidget` (base of dialogs/widgets that show live device data) owns a `CurrentDevice` + a
  `ModuleDataUpdater`; `UDialog` extends it with success/error message plumbing
  (`successMsg`/`errorMsg`) applied automatically when a `DataGeneralResponse` comes back.
- `XmlConfigLoader` locates and loads the right `settings/*.xml` for the connected device and fills
  `CurrentDevice`'s `ConfigStorage`/`S2DataManager` via the XML/S2 parsers.
- `DialogCreator::createDialogs` builds the `UDialog` instances driven by that XML config;
  `DialogManager::setupUI` invokes it after `CurrentDevice` is ready and starts the timer that calls
  `UDialog::reqUpdate` on the active dialog. `Coma::disconnectAndClear` tears this down in reverse:
  `DialogManager::clearDialogs` → `DialogCreator::deleteDialogs` → `ConnectionManager::breakConnection`.

### Internal library dependency order

`gen → s2 → interfaces → {settings, device, models} → widgets → {datablocks, alarms, startup,
oscillograms, xml} → {tune, journals} → dialogs → coma_core → {AVM-Debug, AVM-Service}` (see the last
diagram in `doc/coma_2.11.0.pdf`). Respect this direction when adding new cross-module dependencies —
e.g. `datablocks`/`tune` may depend on `widgets`, but not vice versa.

### External dependencies worth knowing about

- `libavm-gen`, `libavm-widgets` — AVM-Energo's own shared utility/widget libraries (settings, logging,
  error queue, common Qt widgets like `EMessageBox`, `EStatusBar`, `LBLFunc`, `WDFunc`), fetched from
  `git.avmenergo.ru`. Not part of this repo; treat their headers as external API.
- `magic_enum`, `limereport` (reports), `qcustomplot` (plotting), `qxlsx` (Excel export) — vendored via
  CMake `FetchContent`, see `cmake/*.cmake`.
