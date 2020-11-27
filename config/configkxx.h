#ifndef CONFIGKXX_H
#define CONFIGKXX_H

#include "../gen/datatypes.h"
#include "../module/module_kxx.h"

class ConfigKxx : public QObject
{
    Q_OBJECT
public:
    ConfigKxx(S2DataTypes::S2ConfigType *config, QObject *parent = nullptr);

    void setConfig(S2DataTypes::S2ConfigType *config);
    void SetDefConf();
    void Fill();
    void FillBack();
    QWidget *ComParam(QWidget *parent);
    QWidget *ModbusWidget(QWidget *parent);
    QWidget *VariousWidget(QWidget *parent);

private:
    QObject *ParentMB, *ParentSetup, *ParentSetupBl;
    const QStringList m_baudList { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
    QList<QWidget *> WidgetList;

    Bci::TempConfStruct TempConf;
    Bci::StructTrele StrTrele;
    Bci::StructModBus StrModBus {};
    Bci::Com Com_param;

private slots:
    void ChangeModbusGUI(int num);
};

#endif // CONFIGKXX_H
