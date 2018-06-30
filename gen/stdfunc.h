#ifndef STDFUNC_H
#define STDFUNC_H

#define DEVICETYPE_MODULE 1 // модуль
#define DEVICETYPE_DEVICE 2 // прибор

// program sizes
#define PROGSIZE_EMUL   0
#define PROGSIZE_SMALL  1
#define PROGSIZE_MEDIUM 2
#define PROGSIZE_LARGE  3
#define PROGSIZE_FULL   4

#define SIZEMAX     128 // максимальный размер массивов конфигурационных параметров

// thresholds
#define TMAX    FLT_MAX
#define T25     25.0
#define T5      5.0
#define TH1     1.0
#define TH05    0.5
#define TH01    0.1
#define TH005   0.05
#define TH002   0.02
#define TH0005  0.005

// signals
#define S0      0.0
#define S1      1.0
#define S4      4.0
#define S20     20.0


#include <QStringList>
#include <QPointer>
#include <QVector>
#include <QCoreApplication>

class StdFunc
{
public:
    StdFunc();

    static QString MIPIP;
    static QString Port;

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
