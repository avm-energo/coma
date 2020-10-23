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

void AbstractStartupDialog::SetStartupBlock(int blocknum, void *block, quint32 blocksize)
{
    m_startupBlockDescription.num = blocknum;
    m_startupBlockDescription.block = block;
    m_startupBlockDescription.size = blocksize;
}

QWidget *AbstractStartupDialog::buttonWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    QPushButton *pb = new QPushButton("Прочитать из " + tmps);
    connect(pb, SIGNAL(clicked()), this, SLOT(GetCorBdButton()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 1, 1, 2);

    pb = new QPushButton("Записать в модуль");
    connect(pb, SIGNAL(clicked()), this, SLOT(WriteCorBd()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 3, 1, 2);

    // row++;

    row++;

    pb = new QPushButton("Сбросить начальные значения");
    connect(pb, SIGNAL(clicked()), this, SLOT(ResetCor()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 1, 1, 2);

    pb = new QPushButton("Задать начальные значения");
    connect(pb, SIGNAL(clicked()), this, SLOT(WriteCor()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 3, 1, 2);

    // row++;

    row++;

    pb = new QPushButton("Прочитать значения из файла");
    connect(pb, SIGNAL(clicked()), this, SLOT(ReadFromFile()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 1, 1, 2);

    pb = new QPushButton("Сохранить значения в файл");
    connect(pb, SIGNAL(clicked()), this, SLOT(SaveToFile()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, row, 3, 1, 2);
}

void AbstractStartupDialog::GetCorBd() { }

void AbstractStartupDialog::ETHUpdate()
{
    updateFloatData();
    m_updateState = AnswerWasReceived;
}

void AbstractStartupDialog::MBSUpdate() { }

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
        if (Commands::WriteCom(Commands::WriteStartupValues) == Error::Msg::NoError)
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

// void AbstractStartupDialog::MessageOk()
//{
//    QMessageBox::information(this, "INFO", "Записано успешно");
//}

void AbstractStartupDialog::updateFloatData()
{
    QList<DataManager::SignalsStruct> list;
    DataManager::getSignals(MBS_INITREG, MBS_INITREG + 10, DataManager::SignalTypes::FloatWithTime, list);
    if (!list.isEmpty())
    {
        foreach (DataManager::SignalsStruct signal, list)
        {
            DataTypes::FloatWithTimeStruct fwt = qvariant_cast<DataTypes::FloatWithTimeStruct>(signal.data);

            //    if (((Signal)->fl.SigAdr >= MBS_INITREG) && ((Signal)->fl.SigAdr <= 4010))
            //    {
            //        for (int i = 0; i < Signal->SigNumber; i++)
            //        {
            // FillBd(
            //    this, QString::number((Signal + i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal +
            //    i)->fl.SigVal));
            FillBd(this, QString::number(fwt.sigAdr), fwt.sigVal);
        }

        //        if (first)
        QMessageBox::information(this, "INFO", "Прочитано успешно");
        //        else
        //            first = 1;
    }
}

void AbstractStartupDialog::updateStatus()
{
    DataTypes::GeneralResponseStruct grs;
    while (DataManager::getResponse(grs) != Error::Msg::ResEmpty) // get all responses from outList
        TimeFunc::Wait();
    if (grs.type == DataTypes::GeneralResponseTypes::Ok)
        QMessageBox::information(this, "INFO", "Записано успешно");
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

void AbstractStartupDialog::GetCorBdButton()
{
    switch (Board::GetInstance().interfaceType())
    {
    case Board::InterfaceType::USB:
    {
        if (Commands::GetBd(
                m_startupBlockDescription.num, m_startupBlockDescription.block, m_startupBlockDescription.size)
            == Error::Msg::NoError)
        {
            FillCor();
            QMessageBox::information(this, "INFO", "Прочитано успешно");
        }
        break;
    }
    case Board::InterfaceType::RS485:
    {
        ModBus::Information info;
        info.size = (sizeof(CorData) / 4);
        info.adr = 4000;
        emit RS485ReadCorBd(info);
        break;
    }
    case Board::InterfaceType::Ethernet:
    {
        DataManager::reqStartup();
        //        emit CorReadRequest();
        break;
    }
    }
}

void AbstractStartupDialog::ModBusUpdateCorData(QList<ModBus::SignalStruct> Signal)
{
    int i = 0;

    if (Signal.size() > 0)
    {
        if (Signal.at(0).SigAdr == MBS_INITREG)
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

// void AbstractStartupDialog::TimerTimeout() { MessageTimer->stop(); }

void AbstractStartupDialog::ErrorRead() { QMessageBox::information(this, "Ошибка", "Ошибка чтения"); }

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
