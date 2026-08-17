#pragma once

#include <avm-gen/error.h>
#include <device/current_device.h>
#include <interfaces/connectionmanager.h>
#include <interfaces/ifaces/usbhidport.h>
#include <s2/filestruct.h>

#include <QPushButton>
#include <QTemporaryDir>
#include <QWidget>

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
    QTemporaryDir m_tempDir; // хранит скачанные файлы до финальной упаковки в ZIP (см. doc/abatcher-task.md, п.17)
    QString m_zipFileName;

    QPushButton *m_closeButton = nullptr;
    QPushButton *m_startButton = nullptr;
    QPushButton *m_cancelButton = nullptr;
    QWidget *m_progressContainer = nullptr; // сюда добавляются прогресс-бары следующих шагов пакетника
    bool m_isCancelled = false;

    void setupUI();
    void createCloseButton();
    void createStartButton();
    void createCancelButton();
    void createProgressBars();
    
    bool connectToUSB();
    void loadXML();
    void startDownload();
    void cancelDownload();
    void finishDownload();
    void saveBsi();
    void saveBsiExt();
    void requestConfig();
    void saveJournals();
    void saveJournal(const QString &prbName, S2::FilesEnum fileNum, const QString &filename);

private slots:
    void bsiInitFinished(Error::Msg status);
    void saveConfig(Error::Msg status);

signals:
    void finishWork();
};
