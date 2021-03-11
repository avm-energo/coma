#ifndef CONFIGKXX_H
#define CONFIGKXX_H

#include "../gen/datatypes.h"
#include "../module/module_kxx.h"

class ConfigKxx : public QObject
{
    Q_OBJECT
public:
    ConfigKxx(QObject *parent = nullptr);

    void setConfig(S2DataTypes::S2ConfigType *config);
    void setConfig();
    void SetDefConf();
    void Fill();
    void FillBack();
    QWidget *ComParam(QWidget *parent);
    QWidget *ModbusWidget(QWidget *parent);
    QWidget *VariousWidget(QWidget *parent);

private:
    enum ModbusColumns : int
    {
        SensorType = 0,
        BaudRate,
        Parity,
        StopBits,
        Timeout,
        Address,
        FuncCode,
        DataType,
        Register
    };
    QObject *ParentMB, *ParentSetup, *ParentSetupBl;
    const QStringList m_baudList { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
    QStringList m_header { "датчик", "скорость", "чётность", "стопБиты", "период опроса", "абонент", "функция",
        "данные", "регистр" };
    QList<QWidget *> WidgetList;

    Bci::TempConfStruct TempConf;
    Bci::StructTrele StrTrele;
    Bci::StructModBus StrModBus {};
    Bci::Com Com_param;

private slots:
    void ChangeModbusGUI(int num);
};

namespace detail
{

template <typename T> QString splitArray(const T &array, const char symbol)
{
    QString str;
    for (auto it = array.cbegin(); it != array.cend(); it++)
    {
        if (*it != (array.back()))
        {
            str.push_back(QString::number(*it, 10));
            str.push_back(symbol);
        }
        else
            str.push_back(QString::number(*it, 10));
    }
    return str;
}
}
#endif // CONFIGKXX_H
