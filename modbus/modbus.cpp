#include "modbus.h"


ModBus::ModBus(ModBus_Settings Settings, QObject *parent) : QObject(parent)
{
    modbusDevice = new QModbusRtuSerialMaster(this);
    state = new QModbusDevice::State;
    //modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
    //                ui->portEdit->text());

    modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, Settings.parity.toInt());
    modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, Settings.baud.toInt());
    modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, Settings.stop.toInt());
    modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, 8);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, Settings.adr);
    //modbusDevice->setTimeout(m_settingsDialog->settings().responseTime);  // для слэйва не надо
    //modbusDevice->setNumberOfRetries(m_settingsDialog->settings().numberOfRetries);

    //connect(modbusDevice,  SIGNAL(QModbusDevice::stateChanged(QModbusDevice::State state)), parent, SLOT(onModbusStateChanged(state)));

    connect(this,  SIGNAL(ModBusState(QModbusDevice::State*)), parent, SLOT(onModbusStateChanged(QModbusDevice::State*)));

    if(modbusDevice->connectDevice() == true)
    {
       *state = QModbusDevice::ConnectedState;
       emit ModBusState(state);

    }
    else
    {
       *state = QModbusDevice::ClosingState;
       emit ModBusState(state);

    }

}

ModBus::~ModBus()
{
    if(modbusDevice)
    modbusDevice->disconnectDevice();

    delete modbusDevice;

}
