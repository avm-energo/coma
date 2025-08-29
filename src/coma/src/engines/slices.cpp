#include <engines/slices.h>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <interfaces/types/common_types.h>
#include <settings/user_settings.h>

#include <QEventLoop>
#include <QTemporaryDir>

namespace Engines
{

Slices::Slices(Device::CurrentDevice *dev, QObject *parent) : QObject(parent), m_curDev(dev) { }

Error::Msg Slices::CreateSlice()
{
    QTemporaryDir dir;
    QByteArray ba;
    if (dir.isValid())
    {
        m_tempDir.setPath(dir.path()); // set temp path for the files generated in appropriate methods
        // get Bsi
        writeFile("bsi", getBsi());
        // get BsiExt
        if (m_curDev->getConfigStorage()->getDeviceSettings().HaveBSIExt())
            writeFile("bsiext", getBsiExt());
        // get journals
        writeFile("sysjour", getSysJournal());
        if (m_curDev->getConfigStorage()->getDeviceSettings().HaveWorkJournal())
            writeFile("workjour", getWorkJournal());
        if (m_curDev->getConfigStorage()->getDeviceSettings().HaveMeasJournal())
            writeFile("measjour", getMeasJournal());
        // get oscs
        // writeFile("oscs", getOscs());
    }
    return Error::Msg::NoError;
}

QByteArray Slices::getWorkJournal()
{
    return getJournal(Stages::WorkJourLoad, S2::FilesEnum::JourWork);
}

QByteArray Slices::getSysJournal()
{
    return getJournal(Stages::SysJourLoad, S2::FilesEnum::JourSys);
}

QByteArray Slices::getMeasJournal()
{
    return getJournal(Stages::MeasJourLoad, S2::FilesEnum::JourMeas);
}

QByteArray Slices::getJournal(Stages stage, S2::FilesEnum fileNum)
{
    S2::S2BFile file;
    QByteArray ba;
    m_curPRB = stage;
    QMetaObject::Connection rangeConn
        = QObject::connect(m_curDev->sync(), &Interface::SyncConnection::setRange, this, &Slices::setRange);
    QMetaObject::Connection valueConn
        = QObject::connect(m_curDev->sync(), &Interface::SyncConnection::setValue, this, &Slices::setValue);
    m_curDev->sync()->readS2BFileSync(fileNum, file);
    QObject::disconnect(rangeConn);
    QObject::disconnect(valueConn);
    ba.resize(sizeof(file));
    memcpy(&ba.data()[0], &file, sizeof(file));
    return ba;
}

QByteArray Slices::getCurrentState()
{
    return QByteArray();
}

QByteArray Slices::getConfig()
{
    return QByteArray();
}

QByteArray Slices::getStartup()
{
    return QByteArray();
}

QByteArray Slices::getBsi()
{
    setDummyRangeAndValue(BsiLoad, sizeof(m_curDev->bsi()));
    return StdFunc::toByteArray(m_curDev->bsi());
}

QByteArray Slices::getBsiExt()
{
    setDummyRangeAndValue(BsiLoadExt, sizeof(m_curDev->bsiExt()));
    return m_curDev->bsiExt()->toByteArray();
}

QByteArray Slices::getOscs()
{
    QByteArray ba;
    m_tempBA.clear();
    QMetaObject::Connection conn
        = QObject::connect(&m_timeoutTimer, &QTimer::timeout, this, &Slices::oscTechBlockReceivingFinished);
    m_curDev->async()->connection(this, &Slices::oscTechBlockReceived);    // set callback when got S2::OscInfo datatype
    m_curDev->async()->writeCommand(Interface::Commands::C_ReqOscInfo, 1); // initiate osc tech block receiving
    m_timeoutTimer.start(UserSettings::get(UserSettings::ProtocomTimeout));
    m_locker.lock();
    m_somethingHappened.wait(&m_locker);
    m_locker.unlock();
    QObject::disconnect(conn);
    ba.append(m_tempBA);
    //
    while (!oscIds.isEmpty())
    {
        OscInfo info = oscIds.takeFirst();
    }
    /*    if (!loadIfExist(size))
            engine()->currentConnection()->reqFile(
                reqOscNum, DataTypes::FileFormat::CustomS2, size + sizeof(S2::DataRecHeader)); */
    return QByteArray();
}

QByteArray Slices::getTune()
{
    return QByteArray();
}

void Slices::writeFile(const QString &filename, const QByteArray &ba)
{
    if (!ba.isEmpty())
        Files::SaveToFile(m_tempDir.path() + "/" + filename, ba);
}

void Slices::oscTechBlockReceived(const S2::OscInfo &resp)
{
    QByteArray ba(sizeof(S2::OscInfo), Qt::Uninitialized);
    memcpy(ba.data(), &resp, sizeof(S2::OscInfo));
    m_tempBA.append(ba);
    oscIds.append({ resp.typeHeader.id, resp.typeHeader.numByte });
    m_timeoutTimer.start(UserSettings::get(UserSettings::ProtocomTimeout));
}

void Slices::oscTechBlockReceivingFinished()
{
    m_somethingHappened.wakeAll();
}

void Slices::setRange(qint64 range)
{
    emit setProgressRange(m_curPRB, range);
}

void Slices::setValue(qint64 value)
{
    emit setProgressValue(m_curPRB, value);
}

void Slices::setDummyRangeAndValue(Stages stage, qint64 size)
{
    emit setProgressRange(stage, size);
    emit setProgressValue(stage, size);
}
}
