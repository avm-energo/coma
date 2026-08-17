# АВМ-Пакетник — контекст задачи

> Файл-памятка для продолжения работы над подпроектом. Не удалять, обновлять по ходу реализации.
> Описывает **текущее состояние кода**, а не историю его получения — детали расследования прошлых
> багов сюда не идут (они в git-истории), здесь только то, что нужно знать, чтобы дальше работать
> с кодом как он есть сейчас.

## Что это

Подпроект наряду с АВМ-Сервис (`coma`) и АВМ-Наладкой — **АВМ-Пакетник**.
Каталоги: `src/abatcher` (Qt-приложение, класс `Abatcher` — главное окно, класс `ModuleWorker` —
вся логика конвейера) и `src/avm-autobatch` (исполняемый файл, упаковка/сборка,
`avm-autobatcher.rc`, `package/src/avm-autobatcher.tsv`).

Назначение: автоматизированное выполнение последовательности задач на подключённом по USB модуле —
считать текущее состояние (BSI/конфиг/журналы), при необходимости обновить ВПО и конфигурацию до
эталонных значений, проверить результат, упаковать все скачанные файлы в ZIP.

Работа идёт в двух режимах: обычный и **"Пробный запуск"** (чекбокс `m_isDry` / `isDry`, UI —
`Abatcher::createDryCheckBox`). В пробном режиме шаги, которые могут менять состояние устройства
(заливка ВПО, отправка конфигурации) или проверять результат такого изменения, **не выполняются
взаправду** — вместо этого пишется предупреждение в лог, а соответствующие пункты 10-12 и 14-16
(см. список ниже) считаются пройденными успешно.

## Статус: все шаги 1-17 реализованы и работают

Полный цикл (включая настоящее обновление ВПО и полный цикл проверки конфигурации) проверен на
реальном модуле АВМ-КИВ. Ниже — по каждому шагу, что именно реализовано и где, без "ещё не
сделано" — если для какого-то типа модуля функциональность не реализована, это явно написано как
текущее ограничение (см. также «Открытые вопросы» в конце).

## Полный список шагов пакетника

1. **Диалог выбора USB-устройства + галочка "Пробный запуск".**
   `Abatcher::createUSBConnectionsWindow`/`updateUSBConnectionWindow` в
   [abatcher.cpp](../src/abatcher/src/abatcher.cpp), галочка в `createDryCheckBox`.
2. **Подключение к устройству по USB.**
   `ModuleWorker::connectToUSB` ([moduleWorker.cpp:225](../src/abatcher/src/moduleWorker/moduleWorker.cpp#L225)).
   Аналог в АВМ-Сервис: `ConnectDialog` / `Coma::initConnection` ([coma.h:50](../src/coma/include/coma.h#L50)).
3. **Скачать блок Bsi → сохранить `bsi-old`.**
   `ModuleWorker::saveBsi` ([moduleWorker.cpp:309](../src/abatcher/src/moduleWorker/moduleWorker.cpp#L309)).
4. **Скачать блок BsiExt (если есть) → сохранить `bsiext`.**
   `ModuleWorker::saveBsiExt` ([moduleWorker.cpp:324](../src/abatcher/src/moduleWorker/moduleWorker.cpp#L324)) —
   проверяет `HaveBSIExt()`, при отсутствии блока шаг сразу считается пройденным.
5. **Скачать конфигурацию → сохранить `config-old`.**
   `ModuleWorker::requestConfig`/`saveConfig` ([moduleWorker.cpp:347](../src/abatcher/src/moduleWorker/moduleWorker.cpp#L347)).
6. **Скачать журналы → сохранить `sysjour`, `workjour`, `measjour`.**
   `ModuleWorker::saveJournals`/`saveJournal` ([moduleWorker.cpp:395](../src/abatcher/src/moduleWorker/moduleWorker.cpp#L395)) —
   sysjour качается всегда, workjour/measjour — если есть по `HaveWorkJournal()`/`HaveMeasJournal()`.
   Журнал измерений можно отключить вручную — галочка "Скачивать" рядом с его прогресс-баром
   (`ModuleWorker::createMeasJournalRow`, чекбокс `m_measJourCheckBox`).
   - Пункты 3-6 реализованы в `ModuleWorker` напрямую (через `SyncConnection`/`S2DataManager`/
     `FileProvider`), по образцу `Engines::Slices` — [slices.h](../src/coma/include/engines/slices.h) /
     [slices.cpp](../src/coma/src/engines/slices.cpp). Файлы складываются во временную папку
     (`QTemporaryDir m_tempDir`), которая затем целиком архивируется в ZIP на шаге 17
     (`ModuleWorker::finishDownload`) — сам ZIP собирается один раз, в конце всего конвейера, а не
     по шагам 3-6 отдельно.
7. **Скачать настроечные блоки (Bac) → сохранить `bac1`, `bac2`, …**
   `ModuleWorker::collectBacBlocks`/`saveBacBlocks`/`saveBacBlock`
   ([moduleWorker.cpp:446](../src/abatcher/src/moduleWorker/moduleWorker.cpp#L446)).
   - В XML состав Bac-блоков не описан, поэтому список захардкожен по типу модуля — скопирован из
     `DialogCreator::createTuneDialogs` ([dialogcreator.cpp:131-183](../src/coma/src/dialogs/dialogcreator.cpp#L131)),
     единственного места в АВМ-Наладке, где раньше выбирался тип регулировочного блока по плате.
     Протокол — не S2, а устаревший
     `Interface::SyncConnection::reqBlockSync(blocknum, DataTypes::DataBlockTypes::BacBlock, buffer, size)`
     ([sync_connection.h:30](../src/interfaces/include/interfaces/conn/sync_connection.h#L30)), вызывается
     напрямую из `ModuleWorker`, без создания виджет-классов `DataBlock`/UI (классы
     `Bac21`/`Bac82`/`BacA284`/`Bac2A284` используются только как QObject-контейнеры структур — из них
     берётся `block()` → `{blocknum, blocktype, blocksize, pointer}`). Модули КТФ/КДВ/КОТ/МПГ
     Bac-блоков не получают — регулировка для них не реализована и в `DialogCreator` (TODO там же).
8. **Скачать блоки начальных значений → сохранить `startup`.**
   Реализовано только для АВМ-КИВ (по требованию пользователя) — `ModuleWorker::createStartupProgressBar`/
   `saveStartup` ([moduleWorker.cpp:524](../src/abatcher/src/moduleWorker/moduleWorker.cpp#L524)). Для
   остальных типов модулей шаг пропускается (аналогично отсутствующим Bac-блокам).
   - Адрес/количество регистров для КИВ (`KivStartupRegAddr = 4000`, `KivStartupRegCount = 11`)
     захардкожены локально в `moduleWorker.cpp`, взяты из `KIVSTARTUPINITREGR` и
     `sizeof(StartupKIVDialog::CorData)/sizeof(float)` ([startupkivdialog.cpp](../src/coma/src/startup/startupkivdialog.cpp)).
     Порядок регистров совпадает с порядком полей `CorData`, поэтому сырые байты 11 float
     сохраняются в `startup` как есть. Чтение — синхронное, через
     `Interface::SyncConnection::reqStartupSync(addr, count, values)`
     ([sync_connection.h:39](../src/interfaces/include/interfaces/conn/sync_connection.h#L39)/[.cpp](../src/interfaces/src/conn/sync_connection.cpp)),
     добавленный в общую библиотеку `interfaces` по образцу `reqBSI()`/`reqBlockSync()`.
9. **Анализ версии ВПО** — сравнение с актуальной версией для данного модуля.
   Реализовано только для АВМ-КИВ — `ModuleWorker::checkFirmwareVersion`
   ([moduleWorker.cpp:584](../src/abatcher/src/moduleWorker/moduleWorker.cpp#L584)). Для остальных
   типов модулей шаг пропускается (аналогично Bac/Startup).
   - Эталонный файл ВПО для КИВ (`AVM-KIV_v5_0_22.hex`, версия "5.0-0022") зашит в ресурсы
     приложения — [firmware.qrc](../src/abatcher/resources/firmware.qrc), путь
     `src/abatcher/resources/firmware/kiv/AVM-KIV_v5_0_22.hex`, подключён к `avm-autobatcher`
     (см. [avm-autobatch/CMakeLists.txt](../src/avm-autobatch/CMakeLists.txt)) по тому же принципу,
     что и общий `images.qrc`. Номер версии для сравнения — `KivReferenceFwVersion` в
     `moduleWorker.cpp`, в упакованном формате `Device::BsiIndexes::Fwver`/`StdFunc::VerToStr`. Для
     остальных типов модулей источник эталона не определён (см. открытые вопросы).
   - Сравнение версий и шаги 10-12 выполняются только если версия в `bsi-old` (шаг 3) не совпадает с
     эталоном; если версии совпадают, шаги 10-12 сразу считаются пройденными (INFO в лог) без
     повторного скачивания `bsi`.
10. **При несовпадении — принудительная отправка ВПО и переход на новое** (с рестартом связи). В
    "Пробном запуске" — только предупреждение + запись в лог.
11. **Скачать блок Bsi → сохранить `bsi`, сравнить версии ВПО.** В "Пробном запуске" пункт полностью
    пропускается.
12. **При несовпадении — ERROR в лог; при совпадении — INFO:**
    `"Обновление ВПО с версии <old> на версию <new> проведено успешно"`. В "Пробном запуске"
    пропускается.
    - Пункты 10-12 реализованы для КИВ в `ModuleWorker::updateFirmware`
      ([moduleWorker.cpp:627](../src/abatcher/src/moduleWorker/moduleWorker.cpp#L627)). За основу взят
      существующий `FwUploadDialog` — [fwuploaddialog.h](../src/coma/include/dialogs/fwuploaddialog.h) /
      [fwuploaddialog.cpp](../src/coma/src/dialogs/fwuploaddialog.cpp) в АВМ-Сервис (парсинг HEX через
      `S2::HexParser` + `S2Util::StoreDataMem`), переписан в синхронном виде через
      `Interface::SyncConnection::writeFirmwareSync`
      ([sync_connection.h](../src/interfaces/include/interfaces/conn/sync_connection.h)/[.cpp](../src/interfaces/src/conn/sync_connection.cpp)) —
      запись файла ВПО укладывается в обычный синхронный запрос-ответ, подключена к индикации
      прогресса через `SyncConnection::setRange`/`setValue` (тот же приём, что в `saveJournal()`),
      т.к. ~530 КБ пакетами по 64 байта через USB HID может занимать заметное время.
    - Команда `C_StartFirmwareUpgrade` (запуск перехода на записанное ВПО) в обычный синхронный
      запрос-ответ не укладывается — устройство перезаписывает себя и перезагружается, пропадая с
      шины на непредсказуемое время. Отправляется асинхронно (`m_device->async()->writeCommand(...)`,
      без ожидания прямого ответа — так же делает `FwUploadDialog`), после чего `updateFirmware`
      вызывает `Interface::ConnectionManager::reconnect()` (метод сделан `public`) и ждёт сигнал
      `reconnectSuccess` в цикле с паузой `FwUpgradeRetryIntervalMs` (1 сек) до
      `FwUpgradeReconnectTimeoutMs` или отмены пользователем — не опрашивая `reqBSI()` параллельно
      (это обсуждалось и не работает, см. комментарии в коде `updateFirmware()` о взаимодействии с
      `RequestQueue`/`interfaceReconnected()`). Пока идёт ожидание, прогресс-бар "Анализ и обновление
      ВПО" (`prbfwupdate`) переводится в режим "занято" (`PrbFunc::setRange(this, "prbfwupdate", 0)`) —
      тот же приём, что и в `ReconnectDialog` (АВМ-Сервис). По получении `reconnectSuccess` бар
      возвращается в обычный режим, и отдельным прогресс-баром "Чтение Bsi после обновления ВПО"
      (`prbfwbsi`) читается `SyncConnection::reqBSI()` для сравнения версии.
    - Логирование ERROR/INFO/WARNING сделано через стандартные Qt-макросы
      `qInfo()`/`qWarning()`/`qCritical()` — отдельного логгера в проекте нет, это тот же подход, что
      уже использует `FwUploadDialog` (`qCritical()`). В "Пробном запуске" при несовпадении версии,
      кроме `qWarning()`, показывается ещё и `EMessageBox::warning` — оператор должен увидеть это не
      только в логе.
13. **Анализ значений заданных по ID конфигурационных параметров со значениями в модуле.**
    `ModuleWorker::checkConfigParams` ([moduleWorker.cpp:796](../src/abatcher/src/moduleWorker/moduleWorker.cpp#L796)).
    - Источник эталонных значений: в отличие от ВПО (ресурс приложения), эталонные значения для
      конфигурационных параметров захардкожены прямо в `moduleWorker.cpp` (`ReferenceConfigParams`,
      анонимное пространство имён) — по ID: `1035 Tevent_pred=180`, `1036 Tevent_alarm=300`,
      `1037 Trele_pred=180`, `1038 Trele_alarm=300`. Список не привязан к типу модуля (в отличие от
      Bac/Startup/ВПО) — параметры проверяются на любом модуле, у которого они присутствуют в
      конфигурации (`Configuration::contains(id)`); отсутствующие для данного типа модуля ID просто
      игнорируются.
    - Реализация опирается на существующую инфраструктуру `S2::Configuration`/`S2DataManager`:
      эталонное значение по ID оборачивается в `S2::Configuration` через `append(id, QString)` — тот
      же механизм, которым `DataManager::configDataReceive` заполняет `m_defaultConfig` строковыми
      значениями по умолчанию из XML (типы переменных берутся из `S2ConfigStorage`, см.
      `S2DataFactory::create(id, QString)`), поэтому сравнение (`DataItem::operator!=`) корректно
      работает вне зависимости от типа параметра (BYTE/WORD/DWORD/FLOAT/…). `S2::Configuration`
      нельзя возвращать по значению из функции (копирующий конструктор `explicit`) — поэтому
      эталонная конфигурация собирается через `fillReferenceConfig(S2Configuration&)`.
14. **Принудительная установка параметров и отправка конфигурации в модуль.** В "Пробном запуске" —
    только предупреждение + лог.
    - `ModuleWorker::updateConfigParams` ([moduleWorker.cpp:837](../src/abatcher/src/moduleWorker/moduleWorker.cpp#L837))
      переиспользует уже скачанную и разобранную рабочую конфигурацию (`BoardConfiguration::m_workingConfig`,
      заполненную на шаге 5) — подменяются только значения по нужным ID, затем полный бинарный
      конфиг (`S2DataManager::getBinaryConfiguration()`) отправляется через
      `Interface::SyncConnection::writeConfigurationSync`. Эта обёртка вызывает
      `m_connection->writeConfiguration(ba)` (как и рабочий асинхронный путь в
      `ConfigDialog::writeConfig()`), а не общий `writeFile()` напрямую — важно, чтобы
      `silentReconnectMode()` эмитился и для синхронной записи конфигурации тоже (см.
      [sync_connection.cpp](../src/interfaces/src/conn/sync_connection.cpp)).
15. **Скачать конфигурацию → сохранить `config`, проверить заданные параметры.** В "Пробном запуске"
    — пропускается.
16. **При несовпадении хотя бы одного параметра — ERROR в лог; при полном совпадении — INFO:**
    `"Обновление конфигурации проведено успешно"`. В "Пробном запуске" — пропускается.
    - Шаги 15-16 реализованы через `ModuleWorker::verifyConfigParams`/`finishConfigVerify`
      ([moduleWorker.cpp:862](../src/abatcher/src/moduleWorker/moduleWorker.cpp#L862)) — обычная
      **асинхронная** подписка на `S2DataManager::parseStatus` + `FileProvider::request(Config, true)`
      (тот же приём, что и в `requestConfig()`/`saveConfig()` для шага 5, и в
      `ConfigDialog::readConfig()`), с таймаутом через одноразовый `QTimer` (`ConfigVerifyTimeoutMs`,
      не блокирующий цикл) на случай, если ответ не придёт.
    - Модуль применяет только что записанную конфигурацию не мгновенно — если сразу после записи
      значения ещё не совпадают с эталоном, `finishConfigVerify()` не сдаётся сразу, а повторяет
      попытку через `QTimer::singleShot(ConfigVerifyRetryDelayMs, ...)` до `ConfigVerifyMaxAttempts`
      раз, прежде чем окончательно признать несовпадение — тот же эффект, что и известная пауза
      `StdFunc::Wait(1000)` после `writeConfigurationSync` в
      `Tune82Verification::setupNFiltrValue()` ([tune82verification.cpp](../src/coma/src/tune/82/tune82verification.cpp)).
    - **Важно для будущих изменений в этой области**: любой блокирующий цикл ожидания сигнала
      (ручная прокачка `QCoreApplication::processEvents()` в `while`) в этом месте — плохая идея и
      уже приводил к трудноуловимым зависаниям (см. git-историю `moduleWorker.cpp` и
      `doc/abatcher-task.md` в старых коммитах, если нужны детали). Весь остальной код проекта
      (`ConfigDialog`, `avm-debug`, сам `requestConfig()`/`saveConfig()`) всегда ждёт такие сигналы
      обычной асинхронной подпиской — держитесь этого же паттерна.
17. **Упаковка всех сохранённых файлов в один ZIP, диалог сохранения архива, итоговое сообщение об
    успехе/неуспехе всей работы.**
    По клику "Начать скачивание" сразу открывается `FileFunc::chooseFileForSave`
    (`ModuleWorker::startDownload`), путь сохраняется в `m_zipFileName`; по завершении конвейера
    `ModuleWorker::finishDownload` ([moduleWorker.cpp:294](../src/abatcher/src/moduleWorker/moduleWorker.cpp#L294))
    архивирует `m_tempDir` в этот файл через `ZipUtil::CompressDir` и показывает итоговое сообщение.
    Образец упаковки: `Slices::createSlice()` ([slices.cpp:21](../src/coma/src/engines/slices.cpp#L21)) +
    `ZipUtil::CompressDir` ([slices.cpp:48](../src/coma/src/engines/slices.cpp#L48)).
    Флаг `m_isCancelled` используется не только для отмены пользователем, но и как индикатор "что-то
    пошло не так" (по аналогии с `Slices`) — выставляется во всех точках ошибки скачивания/сохранения
    отдельных файлов, чтобы не показать ложное "Архив сохранён" после сбоя.
    **В "Пробном запуске" пункты 10-12 и 14-16 считаются пройденными успешно** (несмотря на то что
    реально не выполнялись).

## Общая структура кода

- `Abatcher` ([abatcher.h](../src/abatcher/include/abatcher.h)/[.cpp](../src/abatcher/src/abatcher.cpp)) —
  главное окно: таблица USB-устройств с автообновлением по таймеру (3 сек), чекбокс "Пробный
  запуск", кнопка "Запуск". По клику создаётся `ModuleWorker`, окно переключается на него через
  `QStackedWidget`. `Abatcher::startModuleWorker` выставляет `MemKeys::maxErrors`/`maxTimeouts`/
  `silentInterval` для USB-соединения (5/5/10000, константа `MemKeys::maxTimeouts` добавлена в
  [names.h](../src/coma/include/common/names.h)) — так же, как АВМ-Сервис в
  `AbstractInterfaceDialog`, чтобы случайные помехи на шине не запускали лишний реконнект.
- `ModuleWorker` ([moduleWorker.h](../src/abatcher/include/moduleWorker/moduleWorker.h)/[.cpp](../src/abatcher/src/moduleWorker/moduleWorker.cpp)) —
  подключается к USB (`connectToUSB`), инициализирует BSI устройства (`Device::CurrentDevice::initBSI`)
  и загружает XML-конфигурацию модуля (`loadXML`). Кнопка "Начать скачивание" (`startDownload`)
  запускает последовательный конвейер шагов 3-16 (см. выше), с прогресс-барами по каждому шагу и
  возможностью отмены (`cancelDownload`). По завершении конвейера (`finishDownload`, шаг 17)
  содержимое временной папки (`QTemporaryDir m_tempDir`) упаковывается в выбранный ZIP. Кнопка
  "Назад" эмитит `finishWork()`, по которому `Abatcher` уничтожает `ModuleWorker` и возвращается к
  списку устройств.
- Конвейер в `startDownload`: `saveBsi` → `saveBsiExt` → `requestConfig`/`saveConfig` →
  `saveJournals` → `collectBacBlocks`/`saveBacBlocks` (докачивают Bac-блоки по хардкоду для
  конкретного типа модуля, складывают в `m_tempDir` как `bac1`, `bac2`, …) → `saveStartup` (для
  АВМ-КИВ докачивает блок начальных значений `startup`, для остальных типов — no-op) →
  `checkFirmwareVersion`/`updateFirmware` (шаги 9-12: для АВМ-КИВ сравнивает версию ВПО из уже
  скачанного `bsi-old` с эталоном из ресурсов и, если нужно и это не "Пробный запуск", обновляет
  ВПО, сохраняя `bsi` после обновления; для остальных типов — no-op) →
  `checkConfigParams`/`updateConfigParams`/`verifyConfigParams` (шаги 13-16: сравнивает значения
  захардкоженных ID конфигурационных параметров с эталоном и, если нужно и это не "Пробный запуск",
  отправляет эталонные значения в модуль, затем асинхронно перечитывает конфигурацию с несколькими
  попытками и проверяет результат, сохраняя `config`) → `finishDownload`.

## Референсы на уже реализованный функционал в АВМ-Сервис (`coma`)

| Шаги | Класс/файл |
|---|---|
| 1-2 (диалог выбора USB, подключение) | `ConnectDialog` ([connectdialog.h](../src/coma/include/dialogs/connectdialog.h)/[.cpp](../src/coma/src/dialogs/connectdialog.cpp)), `Coma::initConnection` ([coma.h:50](../src/coma/include/coma.h#L50)) |
| 3-6 (скачивание Bsi/BsiExt/конфига/журналов) | `Engines::Slices` ([slices.h](../src/coma/include/engines/slices.h)/[slices.cpp](../src/coma/src/engines/slices.cpp)) |
| 7 (Bac) | `DialogCreator::createTuneDialogs` ([dialogcreator.cpp](../src/coma/src/dialogs/dialogcreator.cpp#L131)) — состав блоков по типу платы; `Bac21`/`Bac82`/`BacA284`/`Bac2A284` ([datablocks/{21,82,kiv}](../src/coma/include/datablocks)) — блоки и их `blocknum`/размер |
| 8 (Startup, только КИВ) | `StartupKIVDialog` ([startupkivdialog.h](../src/coma/include/startup/startupkivdialog.h)/[.cpp](../src/coma/src/startup/startupkivdialog.cpp)) — адрес/количество регистров; синхронное чтение через `Interface::SyncConnection::reqStartupSync` ([sync_connection.h](../src/interfaces/include/interfaces/conn/sync_connection.h#L39)) |
| 9-12 (обновление ВПО, только КИВ) | `FwUploadDialog` ([fwuploaddialog.h](../src/coma/include/dialogs/fwuploaddialog.h)/[.cpp](../src/coma/src/dialogs/fwuploaddialog.cpp)) — парсинг HEX и запись ВПО; синхронно реализовано в `ModuleWorker::updateFirmware` через `Interface::SyncConnection::writeFirmwareSync` ([sync_connection.h](../src/interfaces/include/interfaces/conn/sync_connection.h)) + принудительный `Interface::ConnectionManager::reconnect()` после команды перехода |
| 13-16 (конфигурация по ID) | `ConfigDialog` ([configdialog.h](../src/coma/include/dialogs/configdialog.h)/[.cpp](../src/coma/src/dialogs/configdialog.cpp)) — чтение/запись конфигурации, `checkForDiff()`; сама сверка по ID и типам значений — `S2::Configuration`/`S2::DataFactory` ([s2configuration.h](../src/coma/include/s2/s2configuration.h), [s2datafactory.h](../src/coma/include/s2/s2datafactory.h)); реализовано в `ModuleWorker::checkConfigParams`/`updateConfigParams`/`verifyConfigParams` через `S2DataManager`/`FileProvider` + `Interface::SyncConnection::writeConfigurationSync` |
| 17 (упаковка в ZIP) | `Slices::createSlice()` → `ZipUtil::CompressDir` ([slices.cpp:21-48](../src/coma/src/engines/slices.cpp#L21)) |

## Изменения в общих библиотеках (`interfaces`, `coma`), сделанные ради Пакетника

Не специфичны для `abatcher`, затрагивают и АВМ-Сервис — при работе с этим кодом учитывайте, что
он используется не только Пакетником:

- `Interface::SyncConnection::reqStartupSync(addr, count, values)` — синхронное чтение регистров по
  образцу `reqBSI()`/`reqBlockSync()` ([sync_connection.h](../src/interfaces/include/interfaces/conn/sync_connection.h#L39)/[.cpp](../src/interfaces/src/conn/sync_connection.cpp)).
- `Interface::ConnectionManager::reconnect()` сделан `public` (был приватным).
- `MemKeys::maxTimeouts` добавлена в [names.h](../src/coma/include/common/names.h) (не было).
- `Interface::SyncConnection::writeConfigurationSync()` вызывает `m_connection->writeConfiguration(ba)`
  вместо общего `writeFile()` — чтобы синхронная запись конфигурации тоже эмитила
  `silentReconnectMode()`, как и асинхронный путь в `ConfigDialog`.
- `ConnectionContext::init()` ([connectioncontext.cpp](../src/interfaces/src/connectioncontext.cpp)) —
  подписка на `BaseInterface::started` теперь настраивает потоки (`moveToThread`/`QThread::start()`/
  `executor->start()`) только один раз за время жизни контекста соединения (флаг
  `m_threadsStarted`), а не при каждом срабатывании `started()` (в т.ч. при реактивном реконнекте) —
  общий баг гонки потоков при реконнекте, не специфичный для Пакетника.
- `Interface::SyncConnection::reqBSI()` — путь успеха теперь явно выставляет
  `m_responseResult = Error::Msg::NoError` (раньше не выставлял, и поле, не сбрасываемое в `reset()`,
  могло хранить результат постороннего прошлого события).

## Открытые вопросы / TODO

- **Шаги 8-12 (Startup, обновление ВПО) реализованы только для АВМ-КИВ.** Для остальных типов
  модулей (КТФ/КДВ/84/85/21/82 и т.д.):
  - Startup: нужно по аналогии завести адреса/количество регистров для каждого типа (см.
    `startupkdvdialog`/`startupktfdialog`) и, возможно, обобщить `saveStartup` под несколько типов.
  - ВПО: нужно завести свой эталонный `.hex` в ресурсах (или найти общий источник/каталог версий,
    если он появится) и хардкод версии для сравнения, по аналогии с `KivFirmwareResourcePath`/
    `KivReferenceFwVersion`.
- Источник эталонных значений для шагов 9 (версия ВПО) и 13 (конфигурационные параметры по ID) —
  оба захардкожены прямо в `moduleWorker.cpp` (см. выше). Если появится общий каталог/источник
  эталонов в проекте — стоит перейти на него вместо локального хардкода.
- Точные таймауты/число попыток в `verifyConfigParams()`/`finishConfigVerify()`
  (`ConfigVerifyTimeoutMs`/`ConfigVerifyMaxAttempts`/`ConfigVerifyRetryDelayMs`) и в `updateFirmware()`
  (`FwUpgradeReconnectTimeoutMs`/`FwUpgradeRetryIntervalMs`) подобраны по ограниченному числу
  проверок на реальном железе — если на других модулях/условиях проявится более долгая задержка
  (модуль не успевает переподключиться/применить конфигурацию в текущее окно ожидания), стоит их
  пересмотреть в первую очередь.
- Логирования ERROR/INFO/WARNING отдельного от Qt-макросов (`qInfo`/`qWarning`/`qCritical`) в
  проекте нет — если появится общий логгер, шаги 9-16 в `ModuleWorker` стоит на него перевести.
  