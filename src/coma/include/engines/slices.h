#pragma once

#include <device/current_device.h>
#include <gen/error.h>
#include <gen/files.h>

#include <QDir>
#include <QMutex>
#include <QObject>
#include <QTimer>
#include <QWaitCondition>

namespace Engines
{

class Slices : public QObject
{
    Q_OBJECT
public:
    enum Stages
    {
        ConfigLoad,
        BsiLoad,
        BsiLoadExt,
        TuneLoad,
        MeasJourLoad,
        SysJourLoad,
        WorkJourLoad,
        StartupLoad,
        GetCurrentState,
        Save,
        GetOsc
    };
    struct OscInfo
    {
        u32 oscNum;
        u32 oscLength;
    };

    Slices(Device::CurrentDevice *dev, QObject *parent = nullptr);

    Error::Msg CreateSlice();
    void cancel();

signals:
    void setProgressRange(Stages stage, qint64 max);
    void setProgressValue(Stages stage, qint64 value);

private:
    Device::CurrentDevice *m_curDev;
    QDir m_tempDir;
    QWaitCondition m_somethingHappened;
    QMutex m_locker;
    QByteArray m_tempBA;
    QTimer m_timeoutTimer;
    QList<OscInfo> oscIds;
    Stages m_curPRB;

    QByteArray getWorkJournal();
    QByteArray getSysJournal();
    QByteArray getMeasJournal();
    QByteArray getJournal(Stages stage, S2::FilesEnum fileNum);
    QByteArray getCurrentState();
    QByteArray getConfig();
    QByteArray getStartup();
    QByteArray getBsi();
    QByteArray getBsiExt();
    QByteArray getOscs();
    QByteArray getTune();
    void writeFile(const QString &filename, const QByteArray &ba);

private slots:
    void oscTechBlockReceived(const S2::OscInfo &resp);
    void oscTechBlockReceivingFinished();
    void setRange(qint64 range);
    void setValue(qint64 value);
    void setDummyRangeAndValue(Stages stage, qint64 size);
};

}
