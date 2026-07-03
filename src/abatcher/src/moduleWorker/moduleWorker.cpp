#include "moduleWorker/moduleWorker.h"

#include "avm-widgets/emessagebox.h"
#include "interfaces/types/connection_settings.h"
#include <avm-gen/files.h>
#include <avm-gen/files/ziputil.h>
#include <avm-gen/std_ext.h>
#include <avm-gen/stdfunc.h>
#include <avm-widgets/filefunc.h>
#include <avm-widgets/prbfunc.h>
#include <datablocks/21/bac.h>
#include <datablocks/82/bac.h>
#include <datablocks/kiv/bac.h>
#include <datablocks/kiv/bac2.h>
#include <device/device_list.h>
#include <s2/hexparser.h>
#include <s2/s2configuration.h>
#include <s2/s2datatypes.h>
#include <s2/s2util.h>
#include <xml/xmlconfigloader.h>

#include <QCoreApplication>
#include <QDateTime>
#include <QElapsedTimer>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <vector>

namespace
{
// Блок начальных значений реализован только для АВМ-КИВ. Адрес и количество регистров
// захардкожены по образцу StartupKIVDialog (src/coma/src/startup/startupkivdialog.cpp) —
// KIVSTARTUPINITREGR и sizeof(CorData)/sizeof(float) там же.
constexpr quint32 KivStartupRegAddr = 4000;
constexpr quint32 KivStartupRegCount = 11;

// Анализ/обновление ВПО реализовано только для АВМ-КИВ (по требованию пользователя, 2026-07-03).
// Эталонный файл ВПО зашит в ресурсы приложения (src/abatcher/resources/firmware.qrc), источник
// эталонных версий для других типов модулей и общая схема хранения такого эталона пока не определены.
const QString KivFirmwareResourcePath = ":/firmware/kiv/AVM-KIV_v5_0_22.hex";
// Версия "5.0-0022" в формате StdFunc::VerToStr/StrToVer, соответствует файлу AVM-KIV_v5_0_22.hex.
constexpr quint32 KivReferenceFwVersion = (5u << 24) | (0u << 16) | 22u;

// Сколько ждём, пока модуль снова отзовётся после команды перехода на новое ВПО (стирание/запись
// flash-памяти и перезагрузка), и с какой паузой между повторными попытками достучаться до него.
constexpr qint64 FwUpgradeReconnectTimeoutMs = 120000;
constexpr int FwUpgradeRetryIntervalMs = 1000;

// Сколько ждём и с какой паузой перечитываем конфигурацию после записи (шаг 15), пока значения не
// совпадут с эталоном. На реальном модуле обнаружено (2026-07-03): чтение конфигурации сразу после
// успешной writeConfigurationSync возвращает СТАРЫЕ значения — модуль применяет записанную
// конфигурацию не мгновенно, а с некоторой задержкой (аналогично задержке после C_StartFirmwareUpgrade,
// см. п. 9-12). Что задержка реальна и не выдумана — косвенно подтверждается независимо в другом месте
// проекта: Tune82Verification::setupNFiltrValue() (src/coma/src/tune/82/tune82verification.cpp) делает
// StdFunc::Wait(1000) сразу после своего writeConfigurationSync без объяснения зачем.
// Первая оценка (15 секунд суммарно, 5 секунд на попытку) на реальном модуле АВМ-КИВ оказалась
// недостаточной — ни одна из попыток перечитывания не успевала получить ответ вообще (Timeout на
// каждой попытке, а не "значения ещё старые"). Увеличено с запасом; точное время, которое реально
// требуется модулю, всё ещё не измерено - при следующей проверке на железе стоит смотреть в лог
// (см. диагностические qDebug в rereadConfigBlocking/verifyConfigParams) и по факту скорректировать.
constexpr qint64 ConfigVerifyTimeoutMs = 60000;
constexpr int ConfigVerifyRetryIntervalMs = 500;
// Таймаут одной попытки чтения конфигурации (round-trip запроса) - независим от ConfigVerifyTimeoutMs,
// который ограничивает суммарное время всех повторных попыток.
constexpr qint64 ConfigRereadRequestTimeoutMs = 10000;

// Шаги 13-16: анализ и принудительная установка значений заданных по ID конфигурационных
// параметров. Общего источника/каталога эталонных значений по ID в проекте нет (аналогичный вопрос
// для эталонного ВПО решили через ресурс приложения, см. выше) — по решению пользователя
// (2026-07-03) эталонные значения захардкожены прямо здесь.
struct ConfigParamRef
{
    quint32 id;
    QString name;
    QString refValue;
};

const std::vector<ConfigParamRef> ReferenceConfigParams = {
    { 1035, "Tevent_pred", "180" },
    { 1036, "Tevent_alarm", "300" },
    { 1037, "Trele_pred", "180" },
    { 1038, "Trele_alarm", "300" },
};

// Заполняет конфигурацию эталонными значениями (типы записей берутся из storage конкретного модуля -
// тот же механизм, что использует XML-парсер для конфигурации по умолчанию, см. S2DataFactory).
// S2::Configuration нельзя вернуть по значению - копирующий конструктор объявлен explicit.
void fillReferenceConfig(S2Configuration &refConfig)
{
    for (const auto &param : ReferenceConfigParams)
        refConfig.append(param.id, param.refValue);
}

// Возвращает ID эталонных параметров, которые присутствуют в конфигурации модуля (не для всех типов
// модулей заданы все параметры) и значение которых не совпадает с эталонным.
std::vector<quint32> findMismatchedIds(S2DataManager *dataManager, const S2Configuration &refConfig)
{
    std::vector<quint32> mismatched;
    for (auto &[_, boardConfig] : *dataManager)
        for (const auto &param : ReferenceConfigParams)
            if (boardConfig.m_workingConfig.contains(param.id) && boardConfig.m_workingConfig[param.id] != refConfig[param.id])
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
    connect(m_closeButton, &QPushButton::clicked, this, &ModuleWorker::finishWork);
}

void ModuleWorker::createStartButton()
{
    // TODO: в будущем перед стартом здесь будет выбор того, что именно скачивать
    m_startButton = new QPushButton("Начать скачивание", this);
    m_startButton->setEnabled(false);
    connect(m_startButton, &QPushButton::clicked, this, &ModuleWorker::startDownload);
}

void ModuleWorker::createCancelButton()
{
    m_cancelButton = new QPushButton("Отмена", this);
    m_cancelButton->setEnabled(false);
    connect(m_cancelButton, &QPushButton::clicked, this, &ModuleWorker::cancelDownload);
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

void ModuleWorker::startDownload()
{
    QString filename = FileFunc::chooseFileForSave(this, "Zip files (*.zip)", "zip",
        QString("bsi_%1").arg(QDateTime::currentDateTime().toString("yyMMdd_hhmmss")));
    if (filename.isEmpty())
        return;
    m_zipFileName = filename;

    m_isCancelled = false;
    m_startButton->setEnabled(false);
    m_cancelButton->setEnabled(true);
    // "Назад" отключаем на время загрузки: readS2BFileSync крутит цикл событий изнутри,
    // и клик по "Назад" удалил бы ModuleWorker прямо во время выполнения его же методов
    m_closeButton->setEnabled(false);

    saveBsi();
    saveBsiExt();
    requestConfig();
}

void ModuleWorker::cancelDownload()
{
    if (m_isCancelled)
        return;

    m_isCancelled = true;
    m_device->async()->cancelQuery();
    EMessageBox::warning(this, "Скачивание отменено");
    finishDownload();
}

void ModuleWorker::finishDownload()
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
    QByteArray ba = StdFunc::toByteArray(m_device->bsi());
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
        // блока BsiExt у этого типа модуля нет — считаем шаг пройденным
        PrbFunc::setRange(this, "prbbsiext", 1);
        PrbFunc::setValue(this, "prbbsiext", 1);
        return;
    }

    QByteArray ba = m_device->bsiExt()->toByteArray();
    PrbFunc::setRange(this, "prbbsiext", ba.size());

    if (Files::SaveToFile(m_tempDir.path() + "/bsiext", ba) != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Не удалось сохранить блок BsiExt");
        m_isCancelled = true;
        return;
    }

    PrbFunc::setValue(this, "prbbsiext", ba.size());
}

void ModuleWorker::requestConfig()
{
    connect(m_device->getS2Datamanager(), &S2DataManager::parseStatus, this, &ModuleWorker::saveConfig);
    connect(m_device->getFileProvider(), &Device::FileProvider::noConfigurationError, this,
        [this]()
        {
            EMessageBox::error(this, "В модуле отсутствует конфигурация");
            m_isCancelled = true;
            finishDownload();
        });

    m_device->getFileProvider()->request(S2::FilesEnum::Config, true);
}

void ModuleWorker::saveConfig(Error::Msg status)
{
    // saveConfig() относится только к самому первому скачиванию конфигурации (шаг 5, сохраняется как
    // config-old) - отключаемся от parseStatus сразу после срабатывания, чтобы повторный запрос
    // конфигурации на шаге 15 (после принудительной записи эталонных значений, см.
    // updateConfigParams()/verifyConfigParams()) не привёл к повторному вызову этого слота.
    disconnect(m_device->getS2Datamanager(), &S2DataManager::parseStatus, this, &ModuleWorker::saveConfig);

    if (m_isCancelled)
        return;

    if (status != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Не удалось получить конфигурацию модуля");
        m_isCancelled = true;
        finishDownload();
        return;
    }

    QByteArray ba = m_device->getS2Datamanager()->getBinaryConfiguration();
    PrbFunc::setRange(this, "prbconfig", ba.size());

    if (Files::SaveToFile(m_tempDir.path() + "/config-old", ba) != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Не удалось сохранить конфигурацию");
        m_isCancelled = true;
        finishDownload();
        return;
    }

    PrbFunc::setValue(this, "prbconfig", ba.size());
    saveJournals();
}

void ModuleWorker::saveJournals()
{
    saveJournal("prbsysjour", S2::FilesEnum::JourSys, "sysjour");

    if (!m_isCancelled && m_device->getConfigStorage()->getDeviceSettings().HaveWorkJournal())
        saveJournal("prbworkjour", S2::FilesEnum::JourWork, "workjour");

    if (!m_isCancelled && m_measJourCheckBox->isChecked()
        && m_device->getConfigStorage()->getDeviceSettings().HaveMeasJournal())
        saveJournal("prbmeasjour", S2::FilesEnum::JourMeas, "measjour");

    saveBacBlocks();
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

    // Состав настроечных блоков (Bac) в XML не описан, поэтому повторяем хардкод из
    // DialogCreator::createTuneDialogs (АВМ-Наладка, src/coma/src/dialogs/dialogcreator.cpp) —
    // единственного места в проекте, где раньше выбирался тип регулировочного блока по плате.
    // Регулировка модулей КТФ/КДВ/КОТ/МПГ там тоже не реализована (TODO), поэтому Bac-блоков для них нет.
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

    saveStartup();
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
    if (m_isCancelled)
    {
        finishDownload();
        return;
    }

    if (!m_hasStartup)
    {
        checkFirmwareVersion();
        return;
    }

    PrbFunc::setRange(this, "prbstartup", 1);

    QList<float> values;
    if (m_device->sync()->reqStartupSync(KivStartupRegAddr, KivStartupRegCount, values) != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Не удалось скачать блок начальных значений");
        m_isCancelled = true;
        finishDownload();
        return;
    }

    QByteArray ba(reinterpret_cast<const char *>(values.constData()), values.size() * sizeof(float));
    if (Files::SaveToFile(m_tempDir.path() + "/startup", ba) != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Не удалось сохранить блок начальных значений");
        m_isCancelled = true;
        finishDownload();
        return;
    }

    PrbFunc::setValue(this, "prbstartup", 1);
    checkFirmwareVersion();
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

void ModuleWorker::checkFirmwareVersion()
{
    if (m_isCancelled)
    {
        finishDownload();
        return;
    }

    if (!m_hasFwUpdate)
    {
        checkConfigParams();
        return;
    }

    PrbFunc::setRange(this, "prbfwupdate", 1);

    quint32 currentVersion = m_device->bsi().Fwver;
    if (currentVersion == KivReferenceFwVersion)
    {
        qInfo() << QString("Версия ВПО актуальна: %1").arg(StdFunc::VerToStr(currentVersion));
        PrbFunc::setValue(this, "prbfwupdate", 1);
        checkConfigParams();
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
        checkConfigParams();
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
        finishDownload();
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
        finishDownload();
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
        finishDownload();
        return;
    }

    // Запись прошивки идёт через USB HID пакетами по 64 байта, т.е. тысячами мелких транзакций —
    // это может занимать заметное время. Подписываемся на setRange/setValue у SyncConnection (тот же
    // механизм, что и в saveJournal() для скачивания), чтобы прогресс-бар отражал реальный ход записи,
    // а не выглядел зависшим.
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
        finishDownload();
        return;
    }

    // C_StartFirmwareUpgrade не укладывается в обычный синхронный запрос-ответ: устройство стирает
    // и перезаписывает собственную память новым ВПО и перезагружается, пропадая с шины на время,
    // которое протокольным таймаутом одного запроса не покрывается — поэтому отправляем её асинхронно
    // (writeCommand, без ожидания прямого ответа), как это делает FwUploadDialog в АВМ-Сервис.
    m_device->async()->writeCommand(Interface::Commands::C_StartFirmwareUpgrade);

    // На реальном модуле после перепрошивки USB HID соединение (открытый хендл hidapi) перестаёт
    // работать даже после того, как модуль физически появляется на шине снова — простой опрос
    // reqBSI() по старому соединению ни к чему не приводит, т.к. само физическое отключение по USB не
    // всегда доходит до ConnectionManager как явная ошибка интерфейса (проверено на реальном модуле).
    // Поэтому переподключение запускаем сами: ConnectionManager::reconnect() закрывает и заново
    // открывает интерфейс (hid_close/hid_open) в цикле с паузой reconnectInterval, пока модуль не
    // отзовётся или пока мы не сдадимся по таймауту ниже.
    m_connManager->reconnect();

    // ВАЖНО: пока идёт реконнект, свою SyncConnection::reqBSI() параллельно вызывать нельзя —
    // DefaultQueryExecutor::reconnectEvent() (вызывается тем же reconnect()) деактивирует очередь
    // запросов (RequestQueue::deactivate() — входящие запросы в неактивном состоянии игнорируются,
    // см. request_queue.h), а реактивирует её только ConnectionManager::fastCheckBSI() после того,
    // как его СОБСТВЕННЫЙ внутренний пробный reqBSI (отправленный из interfaceReconnected()) получит
    // ответ. Наши параллельные попытки в это окно просто теряются и возвращают Error::Msg::Cancelled —
    // проверено на реальном модуле (устройство видно через hid_enumerate на каждой попытке, но
    // reqBSI стабильно возвращает Cancelled). Поэтому сначала дожидаемся сигнала
    // ConnectionManager::reconnectSuccess (когда очередь снова активна), и только потом сами
    // один раз запрашиваем полный BSI — fastCheckBSI фиксирует успех уже по первому регистру, а нам
    // нужен весь блок целиком (включая Fwver) для сравнения версии.
    bool isReconnected = false;
    auto reconnectConn = connect(m_connManager, &Interface::ConnectionManager::reconnectSuccess, this,
        [&isReconnected] { isReconnected = true; });

    // Заранее неизвестно, сколько секунд займёт переподключение, поэтому вместо счётчика попыток
    // ("N из 120" выглядит неопрятно и мало что говорит оператору) показываем индикатор "занято" —
    // тот же приём, что и в ReconnectDialog (АВМ-Сервис) для точно такого же ожидания реконнекта:
    // QProgressBar с одинаковыми min/max крутится сам по себе, без чисел.
    PrbFunc::setRange(this, "prbfwupdate", 0);

    QElapsedTimer waitTimer;
    waitTimer.start();
    while (!m_isCancelled && !isReconnected && waitTimer.elapsed() < FwUpgradeReconnectTimeoutMs)
        StdFunc::Wait(FwUpgradeRetryIntervalMs);
    disconnect(reconnectConn);

    if (m_isCancelled)
        return; // отменено пользователем во время ожидания перезагрузки модуля после обновления ВПО

    if (!isReconnected)
    {
        qCritical() << "Модуль не переподключился после обновления ВПО в течение отведённого времени";
        EMessageBox::error(this, "Модуль не переподключился после обновления ВПО в течение отведённого времени");
        m_isCancelled = true;
        finishDownload();
        return;
    }

    // Переподключение подтверждено — переводим бар обратно в обычный (не "занято") режим и сразу
    // заполняем до конца, показывая завершённый шаг.
    PrbFunc::setRange(this, "prbfwupdate", 1);
    PrbFunc::setValue(this, "prbfwupdate", 1);

    PrbFunc::setRange(this, "prbfwbsi", 1);
    Error::Msg bsiStatus = m_device->sync()->reqBSI();
    if (bsiStatus != Error::Msg::NoError)
    {
        qCritical() << "Модуль переподключился, но не удалось прочитать блок Bsi после обновления ВПО";
        EMessageBox::error(this, "Модуль переподключился, но не удалось прочитать блок Bsi после обновления ВПО");
        m_isCancelled = true;
        finishDownload();
        return;
    }
    PrbFunc::setValue(this, "prbfwbsi", 1);

    QByteArray bsiBa = StdFunc::toByteArray(m_device->bsi());
    if (Files::SaveToFile(m_tempDir.path() + "/bsi", bsiBa) != Error::Msg::NoError)
    {
        qCritical() << "Не удалось сохранить блок Bsi после обновления ВПО";
        EMessageBox::error(this, "Не удалось сохранить блок Bsi после обновления ВПО");
        m_isCancelled = true;
        finishDownload();
        return;
    }

    if (m_device->bsi().Fwver == KivReferenceFwVersion)
    {
        qInfo() << QString("Обновление ВПО с версии %1 на версию %2 проведено успешно").arg(oldVerStr, newVerStr);
        PrbFunc::setValue(this, "prbfwupdate", 1);
        checkConfigParams();
        return;
    }

    qCritical() << QString("Обновление ВПО не удалось: версия после обновления %1, ожидалась %2")
                       .arg(StdFunc::VerToStr(m_device->bsi().Fwver), newVerStr);
    EMessageBox::error(this, "Обновление ВПО не удалось, версия не изменилась на ожидаемую");
    m_isCancelled = true;
    PrbFunc::setValue(this, "prbfwupdate", 1);
    finishDownload();
}

void ModuleWorker::createConfigCheckProgressBar()
{
    m_progressContainer->layout()->addWidget(PrbFunc::newLBL(
        m_progressContainer, "Анализ и обновление конфигурационных параметров", "prbconfigcheck"));
    m_progressContainer->layout()->addWidget(
        PrbFunc::newLBL(m_progressContainer, "Проверка конфигурации после обновления", "prbconfigverify"));
}

void ModuleWorker::checkConfigParams()
{
    if (m_isCancelled)
    {
        finishDownload();
        return;
    }

    PrbFunc::setRange(this, "prbconfigcheck", 1);

    S2Configuration refConfig(m_device->getS2Datamanager()->getStorage());
    fillReferenceConfig(refConfig);
    auto mismatchedIds = findMismatchedIds(m_device->getS2Datamanager(), refConfig);

    if (mismatchedIds.empty())
    {
        qInfo() << "Значения конфигурационных параметров по ID соответствуют эталону";
        PrbFunc::setValue(this, "prbconfigcheck", 1);
        finishDownload();
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
        finishDownload();
        return;
    }

    qInfo() << QString("Обнаружено несоответствие значений конфигурационных параметров: %1. Отправка эталонных значений.")
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
        finishDownload();
        return;
    }
    PrbFunc::setValue(this, "prbconfigcheck", 1);

    verifyConfigParams();
}

Error::Msg ModuleWorker::rereadConfigBlocking()
{
    // Отдельный от requestConfig()/saveConfig() (шаг 5) разовый запрос конфигурации: тот путь
    // относится только к самому первому скачиванию (config-old) и отключается от parseStatus сам
    // после первого срабатывания, см. saveConfig() - поэтому не мешает повторным запросам здесь.
    //
    // ВАЖНО: соединение обязательно отключается ПОСЛЕ цикла ожидания, а не только внутри самой лямбды
    // при успешном срабатывании. Если бы оно оставалось подключённым после таймаута (когда done так и
    // не стал true), а реальный ответ от модуля пришёл бы позже — лямбда обратилась бы к done/result
    // уже после возврата из этой функции, т.е. к уничтоженным локальным переменным на стеке
    // (undefined behavior; именно так объяснялось предыдущее ложное "Не удалось получить конфигурацию
    // модуля для проверки" при фактически успешном обновлении, см. doc/abatcher-task.md).
    bool done = false;
    Error::Msg result = Error::Msg::Timeout;
    QMetaObject::Connection conn = connect(m_device->getS2Datamanager(), &S2DataManager::parseStatus, this,
        [&done, &result](Error::Msg status)
        {
            result = status;
            done = true;
        });

    m_device->getFileProvider()->request(S2::FilesEnum::Config, true);

    QElapsedTimer waitTimer;
    waitTimer.start();
    while (!done && !m_isCancelled && waitTimer.elapsed() < ConfigRereadRequestTimeoutMs)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait(50);
    }

    disconnect(conn);
    qDebug() << QString("rereadConfigBlocking: done=%1 status=%2 elapsedMs=%3")
                    .arg(done)
                    .arg(done ? int(result) : -1)
                    .arg(waitTimer.elapsed());
    return done ? result : Error::Msg::Timeout;
}

void ModuleWorker::verifyConfigParams()
{
    // Шаг 15-16: перечитать конфигурацию и сравнить с эталоном ещё раз. На реальном модуле сразу
    // после успешной записи (writeConfigurationSync выше) чтение может вернуть ещё старые значения -
    // модуль применяет их не мгновенно (см. ConfigVerifyTimeoutMs/ConfigVerifyRetryIntervalMs выше),
    // поэтому повторяем перечитывание и сравнение до совпадения или истечения таймаута, а не проверяем
    // результат один раз сразу после записи.
    PrbFunc::setRange(this, "prbconfigverify", 0); // "занято" на время повторных попыток - см. prbfwupdate

    std::vector<quint32> stillMismatched;
    Error::Msg status = Error::Msg::NoError;
    QElapsedTimer waitTimer;
    waitTimer.start();
    int attempt = 0;

    do
    {
        ++attempt;
        status = rereadConfigBlocking();
        if (m_isCancelled)
        {
            finishDownload();
            return;
        }

        if (status != Error::Msg::NoError)
        {
            qWarning() << QString("Проверка конфигурации: попытка %1 не удалась (не получен ответ), прошло %2 мс")
                              .arg(attempt)
                              .arg(waitTimer.elapsed());
            continue;
        }

        S2Configuration refConfig(m_device->getS2Datamanager()->getStorage());
        fillReferenceConfig(refConfig);
        stillMismatched = findMismatchedIds(m_device->getS2Datamanager(), refConfig);
        if (stillMismatched.empty())
        {
            qInfo() << QString("Проверка конфигурации: совпадение с эталоном на попытке %1, прошло %2 мс")
                           .arg(attempt)
                           .arg(waitTimer.elapsed());
            break;
        }

        qWarning() << QString("Проверка конфигурации: попытка %1 - значения ещё не совпадают с эталоном (%2), прошло %3 мс")
                          .arg(attempt)
                          .arg(namesForIds(stillMismatched).join(", "))
                          .arg(waitTimer.elapsed());
        StdFunc::Wait(ConfigVerifyRetryIntervalMs);
    } while (waitTimer.elapsed() < ConfigVerifyTimeoutMs);

    PrbFunc::setRange(this, "prbconfigverify", 1);

    if (status != Error::Msg::NoError)
    {
        qCritical() << "Не удалось получить конфигурацию модуля для проверки после обновления";
        EMessageBox::error(this, "Не удалось получить конфигурацию модуля для проверки после обновления");
        m_isCancelled = true;
        finishDownload();
        return;
    }

    QByteArray ba = m_device->getS2Datamanager()->getBinaryConfiguration();
    if (Files::SaveToFile(m_tempDir.path() + "/config", ba) != Error::Msg::NoError)
    {
        qCritical() << "Не удалось сохранить конфигурацию после обновления";
        EMessageBox::error(this, "Не удалось сохранить конфигурацию после обновления");
        m_isCancelled = true;
        finishDownload();
        return;
    }

    if (stillMismatched.empty())
    {
        qInfo() << "Обновление конфигурации проведено успешно";
    }
    else
    {
        qCritical() << QString("Обновление конфигурации не удалось: значения параметров не совпадают с эталоном: %1")
                           .arg(namesForIds(stillMismatched).join(", "));
        EMessageBox::error(this, "Обновление конфигурации не удалось, значения параметров не совпадают с эталоном");
        m_isCancelled = true;
    }

    PrbFunc::setValue(this, "prbconfigverify", 1);
    finishDownload();
}
