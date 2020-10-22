#include "abstractstartupdialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../usb/commands.h"
#include "../widgets/etableview.h"
#include "../widgets/wd_func.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStringListModel>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>

AbstractStartupDialog::AbstractStartupDialog(QWidget *parent) : UDialog(parent)
{
    m_updateState = ThereWasNoUpdatesRecently;
}

void AbstractStartupDialog::GetCorBd()
{
}

void AbstractStartupDialog::ETHUpdate()
{
    updateFloatData();
    m_updateState = AnswerWasReceived;
}

void AbstractStartupDialog::MBSUpdate()
{
}

void AbstractStartupDialog::SetCor()
{
    // if (MainInterface == I_ETHERNET)
    //{
    if (Board::GetInstance().interfaceType() == Board::InterfaceType::Ethernet)
        emit SendCom45(903);
    //}
    // else if (MainInterface == I_USB)
    else if (Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    {
        if (Commands::WriteCom(4) == Error::Msg::NoError)
            QMessageBox::information(this, "INFO", "Записано успешно");
        else
            QMessageBox::information(this, "INFO", "Ошибка");
    }
}

float AbstractStartupDialog::ToFloat(QString text)
{
    bool ok;
    float tmpf;
    tmpf = text.toFloat(&ok);
    if (!ok)
    {
        qCritical() << "Значение " << text << " не может быть переведено во float";
        return 0;
    }
    return tmpf;
}

void AbstractStartupDialog::MessageOk()
{
    QMessageBox::information(this, "INFO", "Записано успешно");
}

void AbstractStartupDialog::updateFloatData()
{
    QList<DataManager::SignalsStruct> list;
    DataManager::getSignals(4000, 4010, DataManager::SignalTypes::FloatWithTime, list);
    if (!list.isEmpty())
    {
        foreach (DataManager::SignalsStruct signal, list)
        {
            DataTypes::FloatWithTime fwt = qvariant_cast<DataTypes::FloatWithTime>(signal.data);

            //    if (((Signal)->fl.SigAdr >= 4000) && ((Signal)->fl.SigAdr <= 4010))
            //    {
            //        for (int i = 0; i < Signal->SigNumber; i++)
            //        {
            // FillBd(
            //    this, QString::number((Signal + i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal +
            //    i)->fl.SigVal));
            FillBd(this, QString::number(fwt.sigAdr), fwt.sigVal);
        }

        if (first)
            QMessageBox::information(this, "INFO", "Прочитано успешно");
        else
            first = 1;
    }
}

void AbstractStartupDialog::FillBd(QWidget *parent, QString Name, QString Value)
{
    bool ok;
    double d = Value.toDouble(&ok);
    if (ok)
    {
        if (!WDFunc::SetSPBData(parent, Name, d))
            qDebug() << "Failed to find SpinBox";
    }
    else
        qDebug() << "Failed to convert" << Value.toFloat();
}

void AbstractStartupDialog::FillBd(QWidget *parent, QString Name, float Value)
{
    if (!WDFunc::SetSPBData(parent, Name, Value))
        qDebug() << "Failed to find SpinBox";
}

void AbstractStartupDialog::ModBusUpdateCorData(QList<ModBus::SignalStruct> Signal)
{
    int i = 0;

    if (Signal.size() > 0)
    {
        if (Signal.at(0).SigAdr == 4000)
        {
            for (i = 0; i < Signal.size(); ++i)
            {
                FillBd(this, QString::number(Signal.at(i).SigAdr), Signal.at(i).flVal);
            }
            QMessageBox::information(this, "INFO", "Прочитано успешно");
        }
    }
}

Error::Msg AbstractStartupDialog::WriteCheckPassword()
{
    KeyPressDialog dlg; // = new KeyPressDialog;
    return dlg.CheckPassword("121941");
}

void AbstractStartupDialog::TimerTimeout()
{
    MessageTimer->stop();
}

void AbstractStartupDialog::ErrorRead()
{
    QMessageBox::information(this, "Ошибка", "Ошибка чтения");
}

void AbstractStartupDialog::update()
{
    if (m_updatesEnabled)
    {
        switch (m_updateState)
        {
        case ThereWasNoUpdatesRecently:
            GetCorBd();
            m_updateState = QueryWasInitiated;
            break;
        case QueryWasInitiated:
        {
            switch (Board::GetInstance().interfaceType())
            {
            case Board::InterfaceType::Ethernet:
                ETHUpdate();
                break;
            case Board::InterfaceType::RS485:
                MBSUpdate();
                break;
            default:
                break;
            }
        }
        break;
        case AnswerWasReceived:
            break;
        }
    }
    else
        m_updateState = ThereWasNoUpdatesRecently;
}
