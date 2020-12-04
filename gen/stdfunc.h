#ifndef STDFUNC_H
#define STDFUNC_H

#include <QString>

#define MAINSLEEP 20
#define MAINTIMEOUT 2000 // 2 sec timeout for general operations

class StdFunc
{
public:
    StdFunc();

    static QString DeviceIP;
    static QString s_OrganizationString;

    static void Init();
    static QString VerToStr(quint32);
    static bool floatIsWithinLimits(double var, double value, double tolerance = 0.1);
    static bool toFloat(const QString &text);
    static void SetHomeDir(const QString &dir);
    static QString GetHomeDir();
    static QString GetSystemHomeDir();
    static void SetDeviceIP(const QString &ip);
    static QString ForDeviceIP();
    static void SetOrganizationString(const QString &str);
    static QString OrganizationString();
    static void setN(int n);
    static int N();
    static void cancel();
    static void clearCancel();
    static bool isCancelled();
    static bool IsInEmulateMode();
    static void SetEmulated(bool tb);
    static int IndexByBit(quint32 dword); // возвращение номера первого, начиная с младшего,
                                          // установленного бита, нумерация с 1, dword=0 => return 0
    static quint32 BitByIndex(int idx); // возвращение битовой маски по индексу (0
                                        // => 0, 1 => 1, 2 => 2, 3 => 4, ...)
    static QString PrbMessage();
    static void SetPrbMessage(const QString &msg);
    static void Wait(int ms = MAINSLEEP);

    static quint32 ping(quint32 addr);
    static quint32 checkPort(quint32 ip4Addr, quint16 port);
    template <typename T> static void joinItem(QList<T> &list, const T item)
    {
        list.append(item);
    }
    static QByteArray compress(const QByteArray &data);
    static bool checkArchiveExist(const QString &path);

    static void removeSubstr(std::string &str, std::string &substr);

    template <typename T> static QByteArray arrayFromNumber(T number)
    {
        QByteArray ba(sizeof(T), 0);
        *(reinterpret_cast<T *>(ba.data())) = number;
        return ba;
    }

private:
    static QString HomeDir;       // рабочий каталог программы
    static QString SystemHomeDir; // системный каталог программы
    static bool Emul;
    static bool Cancelled;
    static QString PrbMsg;
    static int m_N; // степень усреднения для регулировки
};

#endif // STDFUNC_H
