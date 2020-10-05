#ifndef STDFUNC_H
#define STDFUNC_H

#include <QString>

#define MAINSLEEP 20

class StdFunc
{
public:
    StdFunc();

    static QString DeviceIP;
    static QString s_OrganizationString;

    static void Init();
    static QString VerToStr(quint32);
    static bool FloatInRange(float var, float value, float tolerance = 0.1f);
    static void SetHomeDir(const QString &dir);
    static QString GetHomeDir();
    static QString GetSystemHomeDir();
    static void SetDeviceIP(const QString &ip);
    static QString ForDeviceIP();
    static void SetOrganizationString(const QString &str);
    static QString OrganizationString();
    static void Cancel();
    static void ClearCancel();
    static bool IsCancelled();
    static bool IsInEmulateMode();
    static void SetEmulated(bool tb);
    static int IndexByBit(quint32 dword); // возвращение номера первого, начиная с младшего,
                                          // установленного бита, нумерация с 1, dword=0 => return 0
    static quint32 BitByIndex(int idx); // возвращение битовой маски по индексу (0
                                        // => 0, 1 => 1, 2 => 2, 3 => 4, ...)
    static QString PrbMessage();
    static void SetPrbMessage(const QString &msg);
    static void Wait(int ms);

    static quint32 ping(quint32 addr);
    static quint32 checkPort(quint32 ip4Addr, quint16 port);
    template <typename T> static void joinItem(QList<T> &list, const T item)
    {
        list.append(item);
    }

private:
    static QString HomeDir;       // рабочий каталог программы
    static QString SystemHomeDir; // системный каталог программы
    static bool Emul;
    static bool Cancelled;
    static QString PrbMsg;
};

#endif // STDFUNC_H
