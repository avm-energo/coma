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
    QTemporaryDir m_tempDir; // хранит скачанные файлы до финальной упаковки в ZIP (см. doc/abatcher-task.md, п.17)
    QString m_zipFileName;
    QList<DataBlock *> m_bacBlocks; // настроечные блоки (Bac), состав зависит от типа модуля (см. collectBacBlocks)
    bool m_hasStartup = false; // блок начальных значений реализован только для АВМ-КИВ (см. createStartupProgressBar)
    bool m_hasFwUpdate = false; // анализ/принудительное обновление ВПО реализовано только для АВМ-КИВ (см. createFwUpdateProgressBar)

    QPushButton *m_closeButton = nullptr;
    QPushButton *m_startButton = nullptr;
    QPushButton *m_cancelButton = nullptr;
    QWidget *m_progressContainer = nullptr; // сюда добавляются прогресс-бары следующих шагов пакетника
    QCheckBox *m_measJourCheckBox = nullptr; // позволяет пользователю отказаться от скачивания журнала измерений
    bool m_isCancelled = false;

    void setupUI();
    void createCloseButton();
    void createStartButton();
    void createCancelButton();
    void createProgressBars();
    void createMeasJournalRow();

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
    void collectBacBlocks();
    void createBacProgressBars();
    void saveBacBlocks();
    void saveBacBlock(int index);
    void createStartupProgressBar();
    void saveStartup();
    void createFwUpdateProgressBar();
    void checkFirmwareVersion();
    void updateFirmware(const QString &oldVerStr, const QString &newVerStr);
    void createConfigCheckProgressBar();
    void checkConfigParams();
    void updateConfigParams();
    void verifyConfigParams(int attempt);
    void finishConfigVerify(Error::Msg status, int attempt);

private slots:
    void bsiInitFinished(Error::Msg status);
    void saveConfig(Error::Msg status);

signals:
    void finishWork();
};
