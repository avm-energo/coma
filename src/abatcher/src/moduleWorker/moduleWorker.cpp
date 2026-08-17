#include "moduleWorker/moduleWorker.h"
#include "avm-widgets/emessagebox.h"
#include "interfaces/types/connection_settings.h"

#include <avm-gen/files.h>
#include <avm-gen/files/ziputil.h>
#include <avm-gen/stdfunc.h>
#include <avm-widgets/filefunc.h>
#include <avm-widgets/prbfunc.h>
#include <s2/s2datatypes.h>
#include <xml/xmlconfigloader.h>

#include <QDateTime>
#include <QHBoxLayout>
#include <QVBoxLayout>

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
    m_progressContainer->layout()->addWidget(
        PrbFunc::newLBL(m_progressContainer, "Скачивание журнала измерений", "prbmeasjour", "%v из %m"));
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
    // requestConfig() вызывается один раз за сеанс, повторные подключения не нужны
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

    if (!m_isCancelled && m_device->getConfigStorage()->getDeviceSettings().HaveMeasJournal())
        saveJournal("prbmeasjour", S2::FilesEnum::JourMeas, "measjour");

    finishDownload();
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
