#ifndef STDFUNC_H
#define STDFUNC_H

#include <QString>
#ifdef QT_GUI_LIB
#include <QWidget>
#else
#include <QObject>
#endif
#define MAINSLEEP 20
#define MAINTIMEOUT 2000 // 2 sec timeout for general operations

namespace Qt
{
struct deleteLaterDeletor
{
    void operator()(QObject *object) const
    {
        if (object)
        {
            object->deleteLater();
        }
    }
};
}

template <typename T> using SharedPointer = std::shared_ptr<T>;

template <typename T> using UniquePointer = std::unique_ptr<T, Qt::deleteLaterDeletor>;

class StdFunc
{
public:
    StdFunc();

    static QString DeviceIP;
    static QString s_OrganizationString;

    static void Init();
    static QString VerToStr(quint32);
#ifdef QT_GUI_LIB
    static bool floatIsWithinLimits(
        QWidget *w, double var, double value, double tolerance = 0.1, bool showMessage = true);
#endif
    static float toFloat(const QString &text, bool *ok = nullptr);
    static void SetHomeDir(const QString &dir);
    static QString GetHomeDir();
    static QString GetSystemHomeDir();
    static void SetDeviceIP(const QString &ip);
    static QString ForDeviceIP();
    static void SetOrganizationString(const QString &str);
    static QString OrganizationString();
    static void setTuneRequestCount(int n);
    static int tuneRequestCount();
    static void cancel();
    static void clearCancel();
    static bool isCancelled();
    static void setCancelDisabled();
    static void setCancelEnabled();
    static bool IsInEmulateMode();
    static void SetEmulated(bool tb);
    static int IndexByBit(quint32 dword); // возвращение номера первого, начиная с младшего,
                                          // установленного бита, нумерация с 1, dword=0 => return 0
    static quint32 BitByIndex(int idx); // возвращение битовой маски по индексу (0
                                        // => 0, 1 => 1, 2 => 2, 3 => 4, ...)
                                        //    static QString PrbMessage();
                                        //    static void SetPrbMessage(const QString &msg);
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

    template <typename T> static QVariantList toVariantList(const QList<T> &list)
    {
        QVariantList newList;
        for (const T &item : list)
            newList.push_back(item);
        return newList;
    }
    template <typename T> static QList<T> toValuesList(const QList<T *> &list)
    {
        QList<T> newList;
        newList.reserve(list.size());
        std::transform(std::begin(list), std::end(list), std::back_inserter(newList), [](T *item) { return *item; });
        return newList;
    }

private:
    static QString HomeDir;       // рабочий каталог программы
    static QString SystemHomeDir; // системный каталог программы
    static bool Emul;
    static bool Cancelled, s_cancelEnabled;
    //    static QString PrbMsg;
    static int m_tuneRequestCount; // степень усреднения для регулировки
};

#endif // STDFUNC_H
