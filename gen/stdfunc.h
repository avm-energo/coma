#ifndef STDFUNC_H
#define STDFUNC_H

#include <QStringList>
#include <QPointer>
#include <QVector>
#include <QCoreApplication>

class StdFunc
{
public:
    StdFunc();

    static QString MIPIP;

    static QString VerToStr(quint32);
    static bool FloatInRange(float var, float value);
    static void SetHomeDir(const QString &dir);
    static QString GetHomeDir();
    static QString GetSystemHomeDir();
    static void Cancel();
    static void ClearCancel();
    static bool IsCancelled();
    static bool IsInEmulateMode();
    static void SetEmulated(bool tb);
    static int IndexByBit(quint32 dword); // возвращение номера первого, начиная с младшего, установленного бита, нумерация с 1, dword=0 => return 0
    static quint32 BitByIndex(int idx); // возвращение битовой маски по индексу (0 => 0, 1 => 1, 2 => 2, 3 => 4, ...)
    static QString PrbMessage();
    static void SetPrbMessage(const QString &msg);

private:
    static QString HomeDir; // рабочий каталог программы
    static QString SystemHomeDir; // системный каталог программы
    static bool Emul;
    static bool Cancelled;
    static QString PrbMsg;
};

#endif // STDFUNC_H
