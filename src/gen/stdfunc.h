#ifndef STDFUNC_H
#define STDFUNC_H

#include <QObject>
#include <QString>
#include <cmath>
#include <functional>
#include <memory>
#define MAINSLEEP 20
#define MAINTIMEOUT 2000 // 2 sec timeout for general operations

/*!
    \brief Namespace with internal helper structure
*/
namespace QtHelper
{
/*!
    \brief Functor for auto-deleting
    \details Functor for auto-deleting object, used in UniquePointer as default deletor
*/
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

template <typename T> using UniquePointer = std::unique_ptr<T, QtHelper::deleteLaterDeletor>;

/*!
    \brief Class contains most frequently used general purpose functions
    \details That class contains static general purpose functions
*/
class StdFunc
{
public:
    static QString DeviceIP;
    static QString s_OrganizationString;

    StdFunc();

    static void Init();
    static QString VerToStr(quint32);
    static quint32 StrToVer(const QString &str);
    static bool FloatIsWithinLimits(double var, double base, double tolerance);
    static float ToFloat(const QString &text, bool *ok = nullptr);
    static void SetHomeDir(const QString &dir);
    static QString GetHomeDir();
    static QString GetSystemHomeDir();
    static void SetDeviceIP(const QString &ip);
    static QString ForDeviceIP();
    static void SetOrganizationString(const QString &str);
    static QString OrganizationString();
    static void SetTuneRequestCount(int n);
    static int TuneRequestCount();
    static void Cancel();
    static void ClearCancel();
    static bool IsCancelled();
    static void SetCancelDisabled();
    static void SetCancelEnabled();
    static int IndexByBit(quint32 dword); // возвращение номера первого, начиная с младшего,
                                          // установленного бита, нумерация с 1, dword=0 => return 0
    static quint32 BitByIndex(int idx); // возвращение битовой маски по индексу (0
                                        // => 0, 1 => 1, 2 => 2, 3 => 4, ...)
                                        //    static QString PrbMessage();
                                        //    static void SetPrbMessage(const QString &msg);
    static void Wait(int ms = MAINSLEEP);

    static quint32 Ping(quint32 addr);
    static quint32 CheckPort(quint32 ip4Addr, quint16 port);
    template <typename T> static void joinItem(QList<T> &list, const T item)
    {
        list.append(item);
    }
    static QByteArray Compress(const QByteArray &data);
    static bool CheckArchiveExist(const QString &path);

    static void RemoveSubstr(std::string &str, std::string &substr);

    template <typename T, size_t size = sizeof(T)> static QByteArray ArrayFromNumber(T number)
    {
        QByteArray ba(size, 0);
        *(reinterpret_cast<T *>(ba.data())) = number;
        return ba;
    }

    template <typename T> static QVariantList ToVariantList(const QList<T> &list)
    {
        QVariantList newList;
        for (const T &item : list)
            newList.push_back(item);
        return newList;
    }

    template <typename T> static QList<T> ToValuesList(const QList<T *> &list)
    {
        QList<T> newList;
        newList.reserve(list.size());
        std::transform(std::begin(list), std::end(list), std::back_inserter(newList), [](T *item) { return *item; });
        return newList;
    }

    // recursive function to count set bits
    template <typename T> static int CountSetBits(T N)
    {
        int count = 0;
        for (int i = 0; i < sizeof(T) * 8; i++)
        {
            if (N & (1 << i))
                count++;
        }
        return count;
    }

private:
    static QString HomeDir;        ///< \private Рабочий каталог программы
    static QString SystemHomeDir;  ///< \private Системный каталог программы
    static int m_tuneRequestCount; ///< \private Степень усреднения для регулировки

    static bool Cancelled, s_cancelEnabled;
};

#endif // STDFUNC_H
