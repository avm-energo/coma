#pragma once

#include <device/current_device.h>
#include <interfaces/connectionmanager.h>
#include <interfaces/ifaces/usbhidport.h>
#include <libavm-gen/error.h>
#include <s2/filestruct.h>

#include <QCheckBox>
#include <QList>
#include <QPushButton>
#include <QTemporaryDir>
#include <QWidget>

class DataBlock;

class ModuleWorker : public QWidget
{
    Q_OBJECT

public:
    explicit ModuleWorker(bool isDry, UsbHidSettings *usb, QWidget *parent = nullptr);
    virtual ~ModuleWorker();

private:
    bool m_isDry = false;
    UsbHidSettings *m_usb = nullptr;
    Interface::ConnectionManager *m_connManager = nullptr;
    Interface::AsyncConnection *m_connection = nullptr;
    Device::CurrentDevice *m_device = nullptr;
    QTemporaryDir m_tempDir;
    QString m_zipFileName;
    QList<DataBlock *> m_bacBlocks;
    bool m_hasStartup = false;
    bool m_hasFwUpdate = false;

    QPushButton *m_closeButton = nullptr;
    QPushButton *m_startButton = nullptr;
    QPushButton *m_cancelButton = nullptr;
    QWidget *m_progressContainer = nullptr;
    QCheckBox *m_measJourCheckBox = nullptr;
    bool m_isCancelled = false;
    bool m_fwReconnected = false;

    void setupUI();
    void createCloseButton();
    void createStartButton();
    void createCancelButton();
    void createProgressBars();
    void createMeasJournalRow();

    bool connectToUSB();
    void loadXML();
    void startWork();
    void cancelWork();
    void finishWork();
    void saveBsi();
    void saveBsiExt();
    void requestConfig();
    void saveJournals();
    void saveJournal(const QString &prbName, S2::FilesEnum fileNum, const QString &filename);
    void collectBacBlocks();
    void createBacProgressBars();
    void saveBacBlocks();
    void saveBacBlock(int index);
    void createStartupProgressBar();
    void saveStartup();
    void createFwUpdateProgressBar();
    void checkAndUpdateFirmwareVersion();
    void updateFirmware(const QString &oldVerStr, const QString &newVerStr);
    void createConfigCheckProgressBar();
    void checkAndUpdateConfigParams();
    void updateConfigParams();
    void verifyConfigParams();
    void saveConfig();

    Error::Msg saveToFile(QByteArray ba, QString prbFuncStr, QString filename);

private slots:
    void bsiInitFinished(Error::Msg status);

signals:
    void finishWorker();
};
