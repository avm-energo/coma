#include "stdfunc.h"

#include "board.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFileDialog>
#include <QHostAddress>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QTcpSocket>
#include <QTextCodec>
#include <QTextStream>
#include <QThread>
#include <QTimer>

#define QPROCESS_DEBUG

QString StdFunc::HomeDir = "";       // рабочий каталог программы
QString StdFunc::SystemHomeDir = ""; // системный каталог программы
bool StdFunc::Emul = false;
bool StdFunc::Cancelled = false;
QString StdFunc::PrbMsg = "";
QString StdFunc::DeviceIP = "";
QString StdFunc::s_OrganizationString = "";

StdFunc::StdFunc()
{
}

void StdFunc::Init()
{
    Emul = false;

    SystemHomeDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/" + PROGNAME + "/";
    QDir dir(SystemHomeDir);
    if (!dir.exists())
        dir.mkdir(SystemHomeDir);
    QScopedPointer<QSettings> sets = QScopedPointer<QSettings>(new QSettings("EvelSoft", PROGNAME));
    SetOrganizationString(sets->value("OrganizationString", "Р&К").toString());
    SetDeviceIP(sets->value("DeviceIP", "172.16.11.12").toString());
}

QString StdFunc::VerToStr(quint32 num)
{
    int mv = (num & 0xFF000000) >> 24;
    int lv = (num & 0x00FF0000) >> 16;
    int sv = (num & 0x0000FFFF);
    QString tmpString
        = QString::number(mv, 10) + "." + QString::number(lv, 10) + "-" + QString("%1").arg(sv, 4, 10, QChar('0'));
    return tmpString;
}

bool StdFunc::FloatInRange(float var, float value, float tolerance)
{
    if ((var >= (value - tolerance)) && (var <= (value + tolerance)))
        return true;
    else
        return false;
}

void StdFunc::SetHomeDir(const QString &dir)
{
    HomeDir = dir;
}

QString StdFunc::GetHomeDir()
{
    return HomeDir;
}

QString StdFunc::GetSystemHomeDir()
{
    return SystemHomeDir;
}

void StdFunc::SetDeviceIP(const QString &ip)
{
    DeviceIP = ip;
    QScopedPointer<QSettings> sets = QScopedPointer<QSettings>(new QSettings("EvelSoft", PROGNAME));
    sets->setValue("DeviceIP", ip);
}

QString StdFunc::ForDeviceIP()
{
    return DeviceIP;
}

void StdFunc::SetOrganizationString(const QString &str)
{
    s_OrganizationString = str;
    QScopedPointer<QSettings> sets = QScopedPointer<QSettings>(new QSettings("EvelSoft", PROGNAME));
    sets->setValue("OrganizationString", str);
}

QString StdFunc::OrganizationString()
{
    return s_OrganizationString;
}

void StdFunc::Cancel()
{
    Cancelled = true;
}

void StdFunc::ClearCancel()
{
    Cancelled = false;
}

bool StdFunc::IsCancelled()
{
    return Cancelled;
}

bool StdFunc::IsInEmulateMode()
{
    return Emul;
}

void StdFunc::SetEmulated(bool tb)
{
    Emul = tb;
}

int StdFunc::IndexByBit(quint32 dword)
{
    quint32 bit = 0x00000001;
    for (int i = 0; i < 32; ++i)
        if (dword & bit)
            return (i + 1);
    return 0;
}

quint32 StdFunc::BitByIndex(int idx)
{
    if ((idx == 0) || (idx > 31))
        return 0;
    return (0x00000001 << (idx - 1));
}

QString StdFunc::PrbMessage()
{
    return PrbMsg;
}

void StdFunc::SetPrbMessage(const QString &msg)
{
    PrbMsg = msg;
}

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

/*!
     \brief Ping ip address, return ip address if host is alive or return 0 if host is dead

Platform dependent ping function, ping ip address through cmdline utility, parse cmd output.
If output contains TTL host is alive else host is dead.
Returns ip address if host is alive; otherwise returns 0.
    */
quint32 StdFunc::ping(quint32 addr)
{
    QString exec = "ping";
#ifdef _WIN32
    QString param = "-n";
#elif __linux__
    QString param = "-c";
#endif
    QHostAddress host(addr);
    auto *pingProcess = new QProcess;
    QStringList params { param, "1", host.toString() };

    pingProcess->start(exec, params, QIODevice::ReadOnly);
    if (pingProcess->waitForFinished(100))
    {
        QTextCodec *codec = QTextCodec::codecForMib(2086);
        QString p_stdout = codec->toUnicode(pingProcess->readAllStandardOutput());
        QString p_stderr = codec->toUnicode(pingProcess->readAllStandardError());
        QStringList list = p_stderr.isEmpty() ? p_stdout.split("\r\n", Qt::SkipEmptyParts)
                                              : p_stderr.split("\r\n", Qt::SkipEmptyParts);

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

/*! \brief Check port port for ip4Addr ip address

    \param ip address of host, ip4Addr, port for checking, port
    \return Ip address of host if port is open otherwise return 0;
*/
quint32 StdFunc::checkPort(quint32 ip4Addr, quint16 port)
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
