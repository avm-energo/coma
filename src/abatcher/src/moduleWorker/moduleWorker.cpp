#include "moduleWorker/moduleWorker.h"

#include "interfaces/types/connection_settings.h"
#include "libavm-widgets/emessagebox.h"
#include <datablocks/21/bac.h>
#include <datablocks/82/bac.h>
#include <datablocks/kiv/bac.h>
#include <datablocks/kiv/bac2.h>
#include <device/device_list.h>
#include <libavm-gen/files.h>
#include <libavm-gen/files/ziputil.h>
#include <libavm-gen/std_ext.h>
#include <libavm-gen/stdfunc.h>
#include <libavm-widgets/filefunc.h>
#include <libavm-widgets/prbfunc.h>
#include <s2/hexparser.h>
#include <s2/s2configuration.h>
#include <s2/s2datatypes.h>
#include <s2/s2util.h>
#include <xml/xmlconfigloader.h>

#include <QDateTime>
#include <QElapsedTimer>
#include <QFile>
#include <QHBoxLayout>
#include <QTimer>
#include <QVBoxLayout>
#include <array>
#include <vector>

namespace
{
// Адрес/количество регистров блока начальных значений АВМ-КИВ — см. KIVSTARTUPINITREGR и
// sizeof(StartupKIVDialog::CorData)/sizeof(float) в startupkivdialog.cpp.
constexpr quint32 KivStartupRegAddr = 4000;
constexpr quint32 KivStartupRegCount = 11;

// Эталонный файл ВПО для АВМ-КИВ зашит в ресурсы приложения (resources/firmware.qrc).
constexpr const char *KivFirmwareResourcePath = ":/firmware/kiv/AVM-KIV_v5_0_23.hex";
// Версия "5.0-0023" в формате StdFunc::VerToStr/StrToVer, соответствует файлу AVM-KIV_v5_0_23.hex.
constexpr quint32 KivReferenceFwVersion = (5u << 24) | (0u << 16) | 23u;

// Сколько ждём, пока модуль снова отзовётся после команды перехода на новое ВПО (стирание/запись
// flash-памяти и перезагрузка), и с какой паузой между повторными попытками достучаться до него.
constexpr qint64 FwUpgradeReconnectTimeoutMs = 15000;
constexpr int FwUpgradeRetryIntervalMs = 1000;

// Эталонные значения конфигурационных параметров по ID для шагов 13-16 (общего каталога эталонов
// в проекте нет, поэтому захардкожены здесь).
struct ConfigParamRef
{
    quint32 id;
    const char *name;
    const char *refValue;
};

constexpr std::array<ConfigParamRef, 4> ReferenceConfigParams = { {
    { 1035, "Tevent_pred", "180" },
    { 1036, "Tevent_alarm", "300" },
    { 1037, "Trele_pred", "180" },
    { 1038, "Trele_alarm", "300" },
} };

// S2::Configuration нельзя вернуть по значению — копирующий конструктор объявлен explicit,
// поэтому заполняем уже созданный на вызывающей стороне объект.
void fillReferenceConfig(S2Configuration &refConfig)
{
    for (const auto &param : ReferenceConfigParams)
        refConfig.append(param.id, QString::fromLatin1(param.refValue));
}

std::vector<quint32> findMismatchedIds(S2DataManager *dataManager, const S2Configuration &refConfig)
{
    std::vector<quint32> mismatched;
    for (auto &[_, boardConfig] : *dataManager)
        for (const auto &param : ReferenceConfigParams)
            if (boardConfig.m_workingConfig.contains(param.id)
                && boardConfig.m_workingConfig[param.id] != refConfig[param.id])
                mismatched.push_back(param.id);
    return mismatched;
}

QStringList namesForIds(const std::vector<quint32> &ids)
{
    QStringList names;
    for (quint32 id : ids)
        for (const auto &param : ReferenceConfigParams)
            if (param.id == id)
                names << param.name;
    return names;
}
} // namespace

ModuleWorker::ModuleWorker(bool isDry, UsbHidSettings *usb, QWidget *parent)
    : QWidget(parent)
    , m_isDry(isDry)
    , m_usb(usb)
    , m_connManager(new Interface::ConnectionManager(this))
{
    createCloseButton();
    createStartButton();
    createCancelButton();
    setupUI();

    if (!m_tempDir.isValid())
    {
        EMessageBox::error(this, "Не удалось создать временную папку для сохранения файлов");
        return;
    }

    if (!connectToUSB())
    {
        EMessageBox::error(this, "Ошибка подключения устройства");
        return;
    }

    m_device = Device::DeviceFabric::create(m_connection);
    connect(m_device, &Device::CurrentDevice::initBSIFinished, this, &ModuleWorker::bsiInitFinished);
    m_device->initBSI();
}

ModuleWorker::~ModuleWorker()
{
    m_connManager->breakConnection();
}

void ModuleWorker::setupUI()
{
    QVBoxLayout *mainLyout = new QVBoxLayout(this);

    m_progressContainer = new QWidget(this);
    new QVBoxLayout(m_progressContainer);
    createProgressBars();

    QSize btnSize = m_closeButton->sizeHint()
                        .expandedTo(m_startButton->sizeHint())
                        .expandedTo(m_cancelButton->sizeHint())
                        .grownBy({ 20, 10, 20, 10 });
    m_closeButton->setFixedSize(btnSize);
    m_startButton->setFixedSize(btnSize);
    m_cancelButton->setFixedSize(btnSize);

    QHBoxLayout *buttonsLyout = new QHBoxLayout();
    buttonsLyout->addWidget(m_startButton);
    buttonsLyout->addWidget(m_cancelButton);
    buttonsLyout->addWidget(m_closeButton);
    buttonsLyout->addStretch(1);

    mainLyout->addStretch(1);
    mainLyout->addWidget(m_progressContainer, 0, Qt::AlignLeft);
    mainLyout->addStretch(1);
    mainLyout->addLayout(buttonsLyout);

    setLayout(mainLyout);
}

void ModuleWorker::createCloseButton()
{
    m_closeButton = new QPushButton("Назад", this);
    connect(m_closeButton, &QPushButton::clicked, this, &ModuleWorker::finishWorker);
}

void ModuleWorker::createStartButton()
{
    // TODO: в будущем перед стартом здесь будет выбор того, что именно скачивать
    m_startButton = new QPushButton("Начать работу", this);
    m_startButton->setEnabled(false);
    connect(m_startButton, &QPushButton::clicked, this, &ModuleWorker::startWork);
}

void ModuleWorker::createCancelButton()
{
    m_cancelButton = new QPushButton("Отмена", this);
    m_cancelButton->setEnabled(false);
    connect(m_cancelButton, &QPushButton::clicked, this, &ModuleWorker::cancelWork);
}

void ModuleWorker::createProgressBars()
{
    m_progressContainer->layout()->addWidget(PrbFunc::newLBL(m_progressContainer, "Сохранение блока Bsi", "prbbsi"));
    m_progressContainer->layout()->addWidget(
        PrbFunc::newLBL(m_progressContainer, "Сохранение блока BsiExt", "prbbsiext"));
    m_progressContainer->layout()->addWidget(
        PrbFunc::newLBL(m_progressContainer, "Сохранение конфигурации", "prbconfig"));
    m_progressContainer->layout()->addWidget(
        PrbFunc::newLBL(m_progressContainer, "Скачивание системного журнала", "prbsysjour", "%v из %m"));
    m_progressContainer->layout()->addWidget(
        PrbFunc::newLBL(m_progressContainer, "Скачивание рабочего журнала", "prbworkjour", "%v из %m"));
    createMeasJournalRow();
}

void ModuleWorker::createMeasJournalRow()
{
    m_measJourCheckBox = new QCheckBox("Скачивать", m_progressContainer);
    m_measJourCheckBox->setChecked(true);

    auto *row = new QWidget(m_progressContainer);
    auto *rowLyout = new QHBoxLayout(row);
    rowLyout->setContentsMargins(0, 0, 0, 0);
    rowLyout->addWidget(PrbFunc::newLBL(row, "Скачивание журнала измерений", "prbmeasjour", "%v из %m"), 1);
    rowLyout->addWidget(m_measJourCheckBox);

    m_progressContainer->layout()->addWidget(row);
}

bool ModuleWorker::connectToUSB()
{
    ConnectionSettings settings { QString(), m_usb };
    m_connection = m_connManager->createConnection(settings);
    return m_connection && (m_connection->getConnectionState() != Interface::State::Disconnect);
}

void ModuleWorker::bsiInitFinished(Error::Msg status)
{
    if (status == Error::Msg::NoError)
    {
        loadXML();
        collectBacBlocks();
        createBacProgressBars();
        createStartupProgressBar();
        createFwUpdateProgressBar();
        createConfigCheckProgressBar();
        m_startButton->setEnabled(true);
    }
    else
    {
        EMessageBox::error(this, "Превышено время ожидания блока BSI");
    }
}

void ModuleWorker::loadXML()
{
    auto cfgLoader = new Xml::ConfigLoader(m_device);
    if (!cfgLoader->loadSettings())
    {
        EMessageBox::error(this,
            "Не удалось найти конфигурацию для модуля.\n"
            "Проверьте журнал сообщений.\n"
            "Доступны минимальные функции.");
    }
    cfgLoader->deleteLater();
}

void ModuleWorker::startWork()
{
    const auto &bsi = m_device->bsi();
    QString filename = FileFunc::chooseFileForSave(this, "Zip files (*.zip)", "zip",
        QString("module_%1%2_%3")
            .arg(bsi.data(Device::BsiIndexes::MTypeB), 2, 16, QChar('0'))
            .arg(bsi.data(Device::BsiIndexes::MTypeM), 2, 16, QChar('0'))
            .arg(QDateTime::currentDateTime().toString("yyMMdd_hhmmss")));
    if (filename.isEmpty())
        return;
    m_zipFileName = filename;

    m_isCancelled = false;
    m_startButton->setEnabled(false);
    m_cancelButton->setEnabled(true);
    // readS2BFileSync крутит цикл событий изнутри — клик "Назад" во время загрузки удалил бы
    // ModuleWorker прямо во время выполнения его же методов, поэтому кнопка на это время отключена.
    m_closeButton->setEnabled(false);

    saveBsi();
    saveBsiExt();
    saveConfig();
    saveJournals();
    saveBacBlocks();
    saveStartup();
    checkAndUpdateFirmwareVersion();
    checkAndUpdateConfigParams();
    finishWork();
}

void ModuleWorker::cancelWork()
{
    if (m_isCancelled)
        return;

    m_isCancelled = true;
    m_device->async()->cancelQuery();
    EMessageBox::warning(this, "Отменена работы");
}

void ModuleWorker::finishWork()
{
    m_cancelButton->setEnabled(false);
    m_closeButton->setEnabled(true);

    if (ZipUtil::CompressDir(m_zipFileName, m_tempDir.path()) != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Не удалось сохранить архив");
        return;
    }

    if (!m_isCancelled)
        EMessageBox::information(this, QString("Архив сохранён: %1").arg(m_zipFileName));
}

void ModuleWorker::saveBsi()
{
    QByteArray ba = m_device->bsi().toByteArray();
    PrbFunc::setRange(this, "prbbsi", ba.size());

    if (Files::SaveToFile(m_tempDir.path() + "/bsi-old", ba) != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Не удалось сохранить блок Bsi");
        m_isCancelled = true;
        return;
    }

    PrbFunc::setValue(this, "prbbsi", ba.size());
}

void ModuleWorker::saveBsiExt()
{
    if (!m_device->getConfigStorage()->getDeviceSettings().HaveBSIExt())
    {
        PrbFunc::setRange(this, "prbbsiext", 1);
        PrbFunc::setValue(this, "prbbsiext", 1);
        return;
    }

    QByteArray ba = m_device->bsiExt().toByteArray();
    PrbFunc::setRange(this, "prbbsiext", ba.size());

    if (Files::SaveToFile(m_tempDir.path() + "/bsiext", ba) != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Не удалось сохранить блок BsiExt");
        m_isCancelled = true;
        return;
    }

    PrbFunc::setValue(this, "prbbsiext", ba.size());
}

void ModuleWorker::saveConfig()
{
    if (m_isCancelled)
        return;

    QByteArray ba;
    Error::Msg err = m_device->sync()->readFileSync(S2::FilesEnum::Config, ba);
    if (err != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Не удалось получить конфигурацию модуля");
        m_isCancelled = true;
        return;
    }

    m_device->getS2Datamanager()->parseS2File(ba);

    PrbFunc::setRange(this, "prbconfig", ba.size());

    if (Files::SaveToFile(m_tempDir.path() + "/config-old", ba) != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Не удалось сохранить конфигурацию");
        m_isCancelled = true;
        return;
    }

    PrbFunc::setValue(this, "prbconfig", ba.size());
}

void ModuleWorker::saveJournals()
{
    if (m_isCancelled)
        return;

    saveJournal("prbsysjour", S2::FilesEnum::JourSys, "sysjour");

    if (m_device->getConfigStorage()->getDeviceSettings().HaveWorkJournal())
        saveJournal("prbworkjour", S2::FilesEnum::JourWork, "workjour");

    if (m_measJourCheckBox->isChecked() && m_device->getConfigStorage()->getDeviceSettings().HaveMeasJournal())
        saveJournal("prbmeasjour", S2::FilesEnum::JourMeas, "measjour");
}

void ModuleWorker::saveJournal(const QString &prbName, S2::FilesEnum fileNum, const QString &filename)
{
    if (m_isCancelled)
        return;

    auto rangeConn = connect(m_device->sync(), &Interface::SyncConnection::setRange, this,
        [this, prbName](qint64 max) { PrbFunc::setRange(this, prbName, max); });
    auto valueConn = connect(m_device->sync(), &Interface::SyncConnection::setValue, this,
        [this, prbName](qint64 value) { PrbFunc::setValue(this, prbName, value); });

    S2::S2BFile file;
    Error::Msg status = m_device->sync()->readS2BFileSync(fileNum, file);

    disconnect(rangeConn);
    disconnect(valueConn);

    if (m_isCancelled)
        return; // отменено пользователем во время скачивания — ошибку не показываем

    if (status != Error::Msg::NoError)
    {
        EMessageBox::error(this, QString("Не удалось скачать журнал %1").arg(filename));
        m_isCancelled = true;
        return;
    }

    QByteArray ba(reinterpret_cast<const char *>(&file.header), sizeof(S2::S2BFileHeader));
    ba.append(file.data);
    ba.append(QByteArray(reinterpret_cast<const char *>(&file.tail), sizeof(S2::S2BFileTail)));

    if (Files::SaveToFile(m_tempDir.path() + "/" + filename, ba) != Error::Msg::NoError)
    {
        EMessageBox::error(this, QString("Не удалось сохранить журнал %1").arg(filename));
        m_isCancelled = true;
    }
}

void ModuleWorker::collectBacBlocks()
{
    using namespace Device;

    // Состав Bac-блоков в XML не описан, поэтому повторяем хардкод из
    // DialogCreator::createTuneDialogs (src/coma/src/dialogs/dialogcreator.cpp).
    const auto &settings = m_device->getConfigStorage()->getDeviceSettings();
    bool isBoxModule = settings.getFeatures()["isBoxModule"] == "1";
    auto boxModel = static_cast<Model>(m_device->getDeviceType());
    auto typeB = static_cast<BaseBoard>(m_device->getBaseType());
    auto typeM = static_cast<MezzanineBoard>(m_device->getMezzType());

    if (isBoxModule)
    {
        if (boxModel == Model::KIV)
            m_bacBlocks << new Bac2A284(this) << new BacA284(this);
    }
    else
    {
        if (typeB == BaseBoard::MTB_80)
        {
            if (typeM == MezzanineBoard::MTM_81 || typeM == MezzanineBoard::MTM_82 || typeM == MezzanineBoard::MTM_83)
                m_bacBlocks << new Bac82(this);
            else if (typeM == MezzanineBoard::MTM_84)
                m_bacBlocks << new Bac2A284(this);
        }
        if (typeB == BaseBoard::MTB_21)
            m_bacBlocks << new Bac21(1, this);
        if (typeM == MezzanineBoard::MTM_21)
            m_bacBlocks << new Bac21(2, this);
    }
}

void ModuleWorker::createBacProgressBars()
{
    for (int i = 0; i < m_bacBlocks.size(); ++i)
        m_progressContainer->layout()->addWidget(PrbFunc::newLBL(
            m_progressContainer, QString("Сохранение блока Bac%1").arg(i + 1), QString("prbbac%1").arg(i + 1)));
}

void ModuleWorker::saveBacBlocks()
{
    for (int i = 0; i < m_bacBlocks.size() && !m_isCancelled; ++i)
        saveBacBlock(i);
}

void ModuleWorker::saveBacBlock(int index)
{
    DataBlock *dataBlock = m_bacBlocks[index];
    dataBlock->setup(m_device->getUID(), m_device->sync());

    DataBlock::BlockStruct block = dataBlock->block();
    QString prbName = QString("prbbac%1").arg(index + 1);
    PrbFunc::setRange(this, prbName, 1);

    if (m_device->sync()->reqBlockSync(block.blocknum, block.blocktype, block.block, block.blocksize)
        != Error::Msg::NoError)
    {
        EMessageBox::error(this, QString("Не удалось скачать блок Bac%1").arg(index + 1));
        m_isCancelled = true;
        return;
    }

    QByteArray ba(static_cast<char *>(block.block), block.blocksize);
    if (Files::SaveToFile(m_tempDir.path() + "/" + QString("bac%1").arg(index + 1), ba) != Error::Msg::NoError)
    {
        EMessageBox::error(this, QString("Не удалось сохранить блок Bac%1").arg(index + 1));
        m_isCancelled = true;
        return;
    }

    PrbFunc::setValue(this, prbName, 1);
}

void ModuleWorker::createStartupProgressBar()
{
    m_hasStartup = static_cast<Device::Model>(m_device->getDeviceType()) == Device::Model::KIV;
    if (!m_hasStartup)
        return;

    m_progressContainer->layout()->addWidget(
        PrbFunc::newLBL(m_progressContainer, "Сохранение блока начальных значений", "prbstartup"));
}

void ModuleWorker::saveStartup()
{
    if (m_isCancelled || !m_hasStartup)
        return;

    PrbFunc::setRange(this, "prbstartup", 1);

    QList<float> values;
    if (m_device->sync()->reqStartupSync(KivStartupRegAddr, KivStartupRegCount, values) != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Не удалось скачать блок начальных значений");
        m_isCancelled = true;
        return;
    }

    QByteArray ba(reinterpret_cast<const char *>(values.constData()), values.size() * sizeof(float));
    if (Files::SaveToFile(m_tempDir.path() + "/startup", ba) != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Не удалось сохранить блок начальных значений");
        m_isCancelled = true;
        return;
    }

    PrbFunc::setValue(this, "prbstartup", 1);
}

void ModuleWorker::createFwUpdateProgressBar()
{
    m_hasFwUpdate = static_cast<Device::Model>(m_device->getDeviceType()) == Device::Model::KIV;
    if (!m_hasFwUpdate)
        return;

    m_progressContainer->layout()->addWidget(
        PrbFunc::newLBL(m_progressContainer, "Анализ и обновление ВПО", "prbfwupdate", "%v из %m"));
    m_progressContainer->layout()->addWidget(
        PrbFunc::newLBL(m_progressContainer, "Чтение Bsi после обновления ВПО", "prbfwbsi"));
}

void ModuleWorker::checkAndUpdateFirmwareVersion()
{
    if (m_isCancelled || !m_hasFwUpdate)
        return;

    PrbFunc::setRange(this, "prbfwupdate", 1);

    quint32 currentVersion = m_device->bsi().data(Device::BsiIndexes::Fwver);
    if (currentVersion == KivReferenceFwVersion)
    {
        qInfo() << QString("Версия ВПО актуальна: %1").arg(StdFunc::VerToStr(currentVersion));
        PrbFunc::setValue(this, "prbfwupdate", 1);
        return;
    }

    QString oldVerStr = StdFunc::VerToStr(currentVersion);
    QString newVerStr = StdFunc::VerToStr(KivReferenceFwVersion);

    if (m_isDry)
    {
        QString msg = QString("Обнаружено несоответствие версии ВПО (текущая %1, актуальная %2)."
                              " Обновление ВПО не выполняется — пробный запуск.")
                          .arg(oldVerStr, newVerStr);
        qWarning() << msg;
        EMessageBox::warning(this, msg);
        PrbFunc::setValue(this, "prbfwupdate", 1);
        return;
    }

    updateFirmware(oldVerStr, newVerStr);
}

void ModuleWorker::updateFirmware(const QString &oldVerStr, const QString &newVerStr)
{
    QFile hexFile(KivFirmwareResourcePath);
    if (!hexFile.open(QIODevice::ReadOnly))
    {
        qCritical() << "Не удалось открыть встроенный файл ВПО" << KivFirmwareResourcePath;
        EMessageBox::error(this, "Не удалось открыть встроенный файл ВПО");
        m_isCancelled = true;
        return;
    }
    QByteArray hexData = hexFile.readAll();
    hexFile.close();

    S2::HexParser parser;
    bool hexError = false;
    connect(&parser, &S2::HexParser::error, this, [&hexError](S2::HexParseError) { hexError = true; });

    parser.parseFile(hexData);
    auto s2array = parser.getS2Format();
    if (hexError)
    {
        qCritical() << "Встроенный файл ВПО повреждён";
        EMessageBox::error(this, "Встроенный файл ВПО повреждён");
        m_isCancelled = true;
        return;
    }

    constexpr auto fileType = std_ext::to_underlying(S2::FilesEnum::Firmware);
    QByteArray firmware;
    S2Util::StoreDataMem(firmware, s2array, fileType);
    if (firmware.isEmpty())
    {
        qCritical() << "Встроенный файл ВПО имеет некорректный размер";
        EMessageBox::error(this, "Встроенный файл ВПО имеет некорректный размер");
        m_isCancelled = true;
        return;
    }

    // Запись идёт через USB HID пакетами по 64 байта (~530 КБ — тысячи транзакций), поэтому
    // подписываемся на setRange/setValue у SyncConnection, как и в saveJournal(), чтобы прогресс-бар
    // отражал реальный ход записи.
    auto rangeConn = connect(m_device->sync(), &Interface::SyncConnection::setRange, this,
        [this](qint64 max) { PrbFunc::setRange(this, "prbfwupdate", max); });
    auto valueConn = connect(m_device->sync(), &Interface::SyncConnection::setValue, this,
        [this](qint64 value) { PrbFunc::setValue(this, "prbfwupdate", value); });

    Error::Msg fwWriteStatus = m_device->sync()->writeFirmwareSync(firmware);

    disconnect(rangeConn);
    disconnect(valueConn);

    if (fwWriteStatus != Error::Msg::NoError)
    {
        qCritical() << "Не удалось записать ВПО в память модуля";
        EMessageBox::error(this, "Не удалось записать ВПО в память модуля");
        m_isCancelled = true;
        return;
    }

    // Команда перехода на новое ВПО не укладывается в обычный синхронный запрос-ответ: устройство
    // стирает себя и перезагружается, пропадая с шины — поэтому отправляем её асинхронно
    // (writeCommand, без ожидания ответа), как и FwUploadDialog в АВМ-Сервис.
    m_device->async()->writeCommand(Interface::Commands::C_StartFirmwareUpgrade);

    // Перед реконектом ждем, потому что команда не успеет обработаться
    StdFunc::Wait(1000);

    // Старый USB HID хендл сам не оживает после перезагрузки модуля — переподключение (закрытие и
    // повторное открытие интерфейса) запускаем сами через ConnectionManager::reconnect().
    m_connManager->reconnect();

    // Пока идёт реконнект, очередь запросов деактивирована (см. RequestQueue/interfaceReconnected()) и
    // реактивируется только внутренним пробным reqBSI самого ConnectionManager — параллельный
    // SyncConnection::reqBSI() отсюда в это окно вернул бы Error::Msg::Cancelled. Поэтому ждём сигнал
    // reconnectSuccess и только потом сами один раз запрашиваем полный BSI (нужен целиком, включая
    // Fwver, для сравнения версии).
    m_fwReconnected = false;
    auto reconnectConn = connect(
        m_connManager, &Interface::ConnectionManager::reconnectSuccess, this, [this] { m_fwReconnected = true; });

    // Неизвестно заранее, сколько займёт переподключение — переводим прогресс-бар в режим "занято"
    // (min == max крутится сам, без чисел), как и ReconnectDialog в АВМ-Сервис.
    PrbFunc::setRange(this, "prbfwupdate", 0);

    QElapsedTimer waitTimer;
    waitTimer.start();
    while (!m_isCancelled && !m_fwReconnected && waitTimer.elapsed() < FwUpgradeReconnectTimeoutMs)
        StdFunc::Wait(FwUpgradeRetryIntervalMs);
    disconnect(reconnectConn);

    if (m_isCancelled)
        return; // отменено пользователем во время ожидания перезагрузки модуля после обновления ВПО

    if (!m_fwReconnected)
    {
        qCritical() << "Модуль не переподключился после обновления ВПО в течение отведённого времени";
        EMessageBox::error(this, "Модуль не переподключился после обновления ВПО в течение отведённого времени");
        m_isCancelled = true;
        return;
    }

    PrbFunc::setRange(this, "prbfwupdate", 1);
    PrbFunc::setValue(this, "prbfwupdate", 1);

    PrbFunc::setRange(this, "prbfwbsi", 1);
    Error::Msg bsiStatus = m_device->sync()->reqBSI();
    if (bsiStatus != Error::Msg::NoError)
    {
        qCritical() << "Модуль переподключился, но не удалось прочитать блок Bsi после обновления ВПО";
        EMessageBox::error(this, "Модуль переподключился, но не удалось прочитать блок Bsi после обновления ВПО");
        m_isCancelled = true;
        return;
    }
    PrbFunc::setValue(this, "prbfwbsi", 1);

    QByteArray bsiBa = m_device->bsi().toByteArray();
    if (Files::SaveToFile(m_tempDir.path() + "/bsi", bsiBa) != Error::Msg::NoError)
    {
        qCritical() << "Не удалось сохранить блок Bsi после обновления ВПО";
        EMessageBox::error(this, "Не удалось сохранить блок Bsi после обновления ВПО");
        m_isCancelled = true;
        return;
    }

    if (m_device->bsi().data(Device::BsiIndexes::Fwver) == KivReferenceFwVersion)
    {
        qInfo() << QString("Обновление ВПО с версии %1 на версию %2 проведено успешно").arg(oldVerStr, newVerStr);
        PrbFunc::setValue(this, "prbfwupdate", 1);
        return;
    }

    qCritical() << QString("Обновление ВПО не удалось: версия после обновления %1, ожидалась %2")
                       .arg(StdFunc::VerToStr(m_device->bsi().data(Device::BsiIndexes::Fwver)), newVerStr);
    EMessageBox::error(this, "Обновление ВПО не удалось, версия не изменилась на ожидаемую");
    m_isCancelled = true;
    PrbFunc::setValue(this, "prbfwupdate", 1);
}

void ModuleWorker::createConfigCheckProgressBar()
{
    m_progressContainer->layout()->addWidget(
        PrbFunc::newLBL(m_progressContainer, "Анализ и обновление конфигурационных параметров", "prbconfigcheck"));
    m_progressContainer->layout()->addWidget(
        PrbFunc::newLBL(m_progressContainer, "Проверка конфигурации после обновления", "prbconfigverify"));
}

void ModuleWorker::checkAndUpdateConfigParams()
{
    if (m_isCancelled)
        return;

    PrbFunc::setRange(this, "prbconfigcheck", 1);

    S2Configuration refConfig(m_device->getS2Datamanager()->getStorage());
    fillReferenceConfig(refConfig);
    auto mismatchedIds = findMismatchedIds(m_device->getS2Datamanager(), refConfig);

    if (mismatchedIds.empty())
    {
        qInfo() << "Значения конфигурационных параметров по ID соответствуют эталону";
        PrbFunc::setValue(this, "prbconfigcheck", 1);
        return;
    }

    QString namesStr = namesForIds(mismatchedIds).join(", ");

    if (m_isDry)
    {
        QString msg = QString("Обнаружено несоответствие значений конфигурационных параметров: %1."
                              " Отправка конфигурации не выполняется — пробный запуск.")
                          .arg(namesStr);
        qWarning() << msg;
        EMessageBox::warning(this, msg);
        PrbFunc::setValue(this, "prbconfigcheck", 1);
        return;
    }

    qInfo() << QString(
        "Обнаружено несоответствие значений конфигурационных параметров: %1. Отправка эталонных значений.")
                   .arg(namesStr);
    updateConfigParams();
}

void ModuleWorker::updateConfigParams()
{
    S2Configuration refConfig(m_device->getS2Datamanager()->getStorage());
    fillReferenceConfig(refConfig);

    for (auto &[_, boardConfig] : *m_device->getS2Datamanager())
        for (const auto &param : ReferenceConfigParams)
            if (boardConfig.m_workingConfig.contains(param.id))
                boardConfig.m_workingConfig.setRecord(param.id, refConfig[param.id].getData());

    QByteArray ba = m_device->getS2Datamanager()->getBinaryConfiguration();
    if (m_device->sync()->writeConfigurationSync(ba) != Error::Msg::NoError)
    {
        qCritical() << "Не удалось записать конфигурацию в модуль";
        EMessageBox::error(this, "Не удалось записать конфигурацию в модуль");
        m_isCancelled = true;
        PrbFunc::setValue(this, "prbconfigcheck", 1);
        return;
    }

    // Дожидаемся, пока применится конфиг
    StdFunc::Wait(5000);

    PrbFunc::setValue(this, "prbconfigcheck", 1);
    verifyConfigParams();
}

void ModuleWorker::verifyConfigParams()
{
    PrbFunc::setRange(this, "prbconfigverify", 1);

    // Тот же синхронный путь, что и в saveConfig() (шаг 5): readFileSync() сам крутит цикл событий
    // внутри (SyncConnection::eventLoop()) и не возвращается, пока не придёт ответ или не истечёт
    // внутренний таймаут.
    QByteArray fileBa;
    Error::Msg status = m_device->sync()->readFileSync(S2::FilesEnum::Config, fileBa);
    if (status != Error::Msg::NoError)
    {
        qCritical() << "Не удалось получить конфигурацию модуля для проверки после обновления";
        EMessageBox::error(this, "Не удалось получить конфигурацию модуля для проверки после обновления");
        m_isCancelled = true;
        return;
    }

    m_device->getS2Datamanager()->parseS2File(fileBa);

    if (m_isCancelled)
        return;

    S2Configuration refConfig(m_device->getS2Datamanager()->getStorage());
    fillReferenceConfig(refConfig);
    auto stillMismatched = findMismatchedIds(m_device->getS2Datamanager(), refConfig);

    QByteArray ba = m_device->getS2Datamanager()->getBinaryConfiguration();
    if (Files::SaveToFile(m_tempDir.path() + "/config", ba) != Error::Msg::NoError)
    {
        qCritical() << "Не удалось сохранить конфигурацию после обновления";
        EMessageBox::error(this, "Не удалось сохранить конфигурацию после обновления");
        m_isCancelled = true;
        return;
    }

    if (!stillMismatched.empty())
    {
        qCritical() << QString("Обновление конфигурации не удалось: значения параметров не совпадают с эталоном: %1")
                           .arg(namesForIds(stillMismatched).join(", "));
        EMessageBox::error(this, "Обновление конфигурации не удалось, значения параметров не совпадают с эталоном");
        m_isCancelled = true;
    }

    PrbFunc::setValue(this, "prbconfigverify", 1);
}
