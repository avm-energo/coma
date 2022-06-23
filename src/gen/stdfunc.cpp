#include "stdfunc.h"

#include "../gen/error.h"
#include "../gen/s2.h"
#include "pch.h"

#include <QCoreApplication>
#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QHostAddress>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QTcpSocket>
#include <QTextCodec>
#include <QTextStream>
#include <QThread>
#include <QTimer>

QString StdFunc::HomeDir = "";       // Рабочий каталог программы
QString StdFunc::SystemHomeDir = ""; // Системный каталог программы

bool StdFunc::Cancelled = false;
bool StdFunc::s_cancelEnabled = true;
QString StdFunc::DeviceIP = "";
QString StdFunc::s_OrganizationString = "";
int StdFunc::m_tuneRequestCount = 0;

StdFunc::StdFunc()
{
}

/*! \brief Initialization function for static class fields.
 *  \details Initialize next fields by values: system home directory, organization, device IP, etc...
 */
void StdFunc::Init()
{
    SystemHomeDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/"
        + QCoreApplication::applicationName() + "/";
    if ((!SystemHomeDir.contains("/root")) && SystemHomeDir.startsWith("//"))
    {
        if (SystemHomeDir.front() == "/")
            SystemHomeDir.replace(0, 1, "/root");
    }

    QDir dir(SystemHomeDir);
    if (!dir.exists())
        dir.mkdir(SystemHomeDir);
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    SetOrganizationString(sets->value("OrganizationString", "Р&К").toString());
    SetDeviceIP(sets->value("DeviceIP", "172.16.11.12").toString());
    SetTuneRequestCount(sets->value("TuneRequestCount", "20").toInt());
}

/// \brief Converts a version from quint32 datatype to string view.
QString StdFunc::VerToStr(quint32 num)
{
    int mv = (num & 0xFF000000) >> 24;
    int lv = (num & 0x00FF0000) >> 16;
    int sv = (num & 0x0000FFFF);
    QString tmpString
        = QString::number(mv, 10) + "." + QString::number(lv, 10) + "-" + QString("%1").arg(sv, 4, 10, QChar('0'));
    return tmpString;
}

/// \brief Converts a version from string view to quint32 datatype.
quint32 StdFunc::StrToVer(const QString &str)
{
    auto dotPos = str.indexOf('.');
    int mv = str.leftRef(dotPos).toInt() << 24;
    auto dashPos = str.indexOf('-');
    int lv = str.midRef(dotPos + 1, dashPos - dotPos - 1).toInt() << 16;
    auto svStr = str.rightRef(str.size() - dashPos - 1);
    int sv = svStr.toInt();
    return mv | lv | sv;
}

/// \brief Converts a value from string view to float point datatype.
float StdFunc::ToFloat(const QString &text, bool *ok)
{
    bool floatok;
    float tmpf;
    tmpf = text.toFloat(&floatok);
    if (!floatok)
    {
        qCritical() << "Значение " << text << " не может быть переведено во float";
        if (ok != 0)
            *ok = false;
        return 0;
    }
    if (ok != 0)
        *ok = true;
    return tmpf;
}

/*! \brief Checks that the number is in the specified interval.
 *  \param var Checked number.
 *  \param base,tolerance Limits of specified interval.
 */
bool StdFunc::FloatIsWithinLimits(double var, double base, double tolerance)
{
    auto tmpf = fabs(var - base);
    return (tmpf < fabs(tolerance));
}

/// \brief Sets new path for home directory field.
void StdFunc::SetHomeDir(const QString &dir)
{
    HomeDir = dir;
}

/// \brief Returns path for home directory.
QString StdFunc::GetHomeDir()
{
    return HomeDir;
}

/// \brief Returns path for system home directory.
QString StdFunc::GetSystemHomeDir()
{
    return SystemHomeDir;
}

/*! \brief Sets new device's IP.
 *  \param ip String that contains new IP address.
 */
void StdFunc::SetDeviceIP(const QString &ip)
{
    DeviceIP = ip;
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    sets->setValue("DeviceIP", ip);
}

/// \brief Returns device's IP.
QString StdFunc::ForDeviceIP()
{
    return DeviceIP;
}

/*! \brief Sets new organization name.
 *  \param str New organization name in string view.
 */
void StdFunc::SetOrganizationString(const QString &str)
{
    s_OrganizationString = str;
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    sets->setValue("OrganizationString", str);
}

/// \brief Returns organization name.
QString StdFunc::OrganizationString()
{
    return s_OrganizationString;
}

/// \brief Sets new tune request count.
void StdFunc::SetTuneRequestCount(int n)
{
    m_tuneRequestCount = n;
}

/// \brief Returns tune request count.
int StdFunc::TuneRequestCount()
{
    return m_tuneRequestCount;
}

/// \brief Sets cancel state when enabled.
void StdFunc::Cancel()
{
    if (s_cancelEnabled)
        Cancelled = true;
}

/// \brief Turns off cancel state.
void StdFunc::ClearCancel()
{
    Cancelled = false;
}

/// \brief Returns cancel state.
bool StdFunc::IsCancelled()
{
    return Cancelled;
}

/// \brief Disallows to set cancel state.
void StdFunc::SetCancelDisabled()
{
    s_cancelEnabled = false;
}

/// \brief Allows to set cancel state.
void StdFunc::SetCancelEnabled()
{
    s_cancelEnabled = true;
}

/*! \brief Returns the position of first bit set.
 *  \details Returns position of the first '1' starting from LSB.
 *  \param dword 32bit bitstring.
 */
int StdFunc::IndexByBit(quint32 dword)
{
    quint32 bit = 0x00000001;
    for (int i = 0; i < 32; ++i)
        if (dword & bit)
            return (i + 1);
    return 0;
}

/*! \brief Returns the 32bit bitstring by index position.
 *  \details Returns 32bit bitstring with '1' in index position.
 *  \param index Position of '1' from LSB.
 *  \return Example: 0 => 0, 1 => 1, 2 => 2, 3 => 4, ...
 */
quint32 StdFunc::BitByIndex(int index)
{
    quint32 bit = 0x00000001;
    if ((index == 0) || (index > 31))
        return 0;
    return (bit << (index - 1));
}

/// \brief Puts the thread to sleep for a given time in ms.
void StdFunc::Wait(int ms)
{
    QElapsedTimer tmr;
    tmr.start();
    while (tmr.elapsed() < ms)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        QThread::msleep(MAINSLEEP);
    }
}

/*! \brief Ping IP address, return IP address if host is alive or return 0 if host is dead
 *  \details Platform dependent ping function, ping IP address through cmdline utility,
 *  parse cmd output. If output contains TTL host is alive else host is dead.
 *  \return IP address if host is alive, otherwise returns 0.
 */
quint32 StdFunc::Ping(quint32 addr)
{
    QString exec = "ping";
#ifdef Q_OS_WINDOWS
    QString param = "-n";
#endif
#ifdef Q_OS_LINUX
    QString param = "-c";
#endif
    QHostAddress host(addr);
    auto pingProcess = new QProcess;
    QStringList params { param, "1", host.toString() };

    pingProcess->start(exec, params, QIODevice::ReadOnly);
    if (pingProcess->waitForFinished(100))
    {
        QTextCodec *codec = QTextCodec::codecForMib(2086);
        QString p_stdout = codec->toUnicode(pingProcess->readAllStandardOutput());
        QString p_stderr = codec->toUnicode(pingProcess->readAllStandardError());
#if QT_VERSION >= 0x050C00
        QStringList list = p_stderr.isEmpty() ? p_stdout.split("\r\n", Qt::SkipEmptyParts)
                                              : p_stderr.split("\r\n", Qt::SkipEmptyParts);
#else
        QStringList list = p_stderr.isEmpty() ? p_stdout.split("\r\n") : p_stderr.split("\r\n");
#endif
        if (std::any_of(list.constBegin(), list.constEnd(),
                [](const QString &i) { return i.contains("TTL", Qt::CaseInsensitive); }))
        {
            delete pingProcess;
            return addr;
        }
    }
    pingProcess->kill();
    pingProcess->waitForFinished();
    delete pingProcess;
    return 0;
}

/*! \brief Checks port and IPv4 address for connection.
 *  \details Checks if the connection can be made with given IP address and port.
 *  \param ip4Addr[in] IPv4 host address.
 *  \param port[in] Connection port.
 *  \return IPv4 address if connection can be made, 0 otherwise.
 */
quint32 StdFunc::CheckPort(quint32 ip4Addr, quint16 port)
{
    QHostAddress host(ip4Addr);
    QTcpSocket *sock = new QTcpSocket;
    QTimer *timer = new QTimer;
    timer->setInterval(1000);
    QEventLoop *loop = new QEventLoop;

    QObject::connect(sock, &QAbstractSocket::connected, [&]() { loop->quit(); });
    QObject::connect(timer, &QTimer::timeout, [&]() {
        loop->quit();
        ip4Addr = 0;
    });
    timer->start();
    // qDebug() << "Timer started";
    Q_ASSERT(sock != nullptr);
    sock->connectToHost(host, port);
    loop->exec(QEventLoop::ExcludeUserInputEvents);
    sock->disconnect();
    sock->disconnectFromHost();
    delete sock;
    timer->stop();
    delete timer;
    delete loop;
    qDebug() << (ip4Addr ? "Port opened" : "Port closed");
    return ip4Addr;
}

/*! \brief Compress input byte array with zlib.
 *  \param data[in] Input byte array for compressing.
 *  \return Compressed byte array.
 */
QByteArray StdFunc::Compress(const QByteArray &data)
{
    // Compress the buffer (using zlib) in a compression rate at 9
    auto compressedData = qCompress(data, 9);
    //  Strip the first six bytes (a 4-byte length put on by qCompress and a 2-byte zlib header)
    // (see RFC 1950)
    compressedData.remove(0, 6);
    // Remove ADLER-32
    compressedData.chop(4);

    QByteArray header;
    QDataStream headerStream(&header, QIODevice::WriteOnly);
    // (see RFC 1952)
    /* Prepend |ID1|ID2|CM|FLG|
    ID1 (IDentification 1)
    ID2 (IDentification 2)
    These have the fixed values ID1 = 31 (0x1f, \037), ID2 = 139
    (0x8b, \213), to identify the file as being in gzip format.
    CM (Compression Method)
    This identifies the compression method used in the file.
    CM = 8 denotes the "deflate" compression method.
    FLG (FLaGs) do not used here so 0*/
    headerStream << quint16(0x1f8b) << quint16(0x0800);
    headerStream.setByteOrder(QDataStream::LittleEndian);
    /*
     * MTIME (Modification TIME)
    This gives the most recent modification time of the original
    file being compressed.  The time is in Unix format, i.e.,
    seconds since 00:00:00 GMT, Jan.  1, 1970.  (Note that this
    may cause problems for MS-DOS and other systems that use
    local rather than Universal time.)  If the compressed data
    did not come from a file, MTIME is set to the time at which
    compression started.  MTIME = 0 means no time stamp is
    available.
    */
    // Current time
    headerStream << QDateTime::currentDateTime().toSecsSinceEpoch();
    headerStream.setByteOrder(QDataStream::BigEndian);
    //  |XFL|OS|
    // XFL = 0x02 as maximum compression used
    headerStream << quint16(0x0200);
    QByteArray footer;
    QDataStream footerStream(&footer, QIODevice::WriteOnly);
    footerStream.setByteOrder(QDataStream::LittleEndian);
    footerStream << quint32(S2::crc32buf(data)) // Append a four-byte CRC-32 of the uncompressed data
                 << quint32(data.size());       // Append 4 bytes uncompressed input size modulo 2^32
    return header + compressedData + footer;
}

/*! \brief Checks existing archive.
 *  \details Cycle rotating 10 archive files with indexes [0...9], older archive
 *  with index 9 will be deleted. For new archive index 0 will be given.
 *  \param path Path to directory with archives.
 */
bool StdFunc::CheckArchiveExist(const QString &path)
{
    // rotating
    for (int i = 9; i > 0; --i)
    {
        QString tmpsnew = path + "." + QString::number(i) + ".gz";
        QString tmpsold = path + "." + QString::number(i - 1) + ".gz";
        QFile fn;
        fn.setFileName(tmpsnew);
        if (fn.exists())
            fn.remove();
        fn.setFileName(tmpsold);
        if (fn.exists())
        {
            if (fn.rename(tmpsnew) == false) // error
            {
                ERMSG("Cannot rename file");
                return false;
            }
        }
    }
    return true;
}

/*! \brief Removes specified substring from specified string.
 *  \param str[in, out] The string from which the substring will be removed.
 *  \param substr[i] The substring that will be removed from string.
 */
void StdFunc::RemoveSubstr(std::string &str, std::string &substr)
{
    std::string::size_type n = substr.length();
    for (std::string::size_type i = str.find(substr); i != std::string::npos; i = str.find(substr))
        str.erase(i, n);
}
