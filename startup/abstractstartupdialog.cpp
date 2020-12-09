#include "abstractstartupdialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/stdfunc.h"
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

AbstractStartupDialog::AbstractStartupDialog(QWidget *parent) : UDialog(parent)
{
    m_updateState = ThereWasNoUpdatesRecently;
}

void AbstractStartupDialog::SetStartupBlock(int blocknum, void *block, quint32 blocksize, quint32 startAdr)
{
    m_startupBlockDescription.num = blocknum;
    m_startupBlockDescription.block = block;
    m_startupBlockDescription.size = blocksize;
    m_startupBlockDescription.initStartRegAdr = startAdr;
}

QWidget *AbstractStartupDialog::buttonWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");

    const QList<QPair<QPair<QString, QIcon>, std::function<void()>>> funcs {
        { { "Получить" /*из " + tmps*/, QIcon(":/icons/User-download-wf.svg") }, [this]() { GetCorBdButton(); } }, //
        { { "Записать" /*в модуль"*/, QIcon(":/icons/Upload-01-wf.svg") }, [this]() { WriteCorBd(); } },           //
        { { "Сбросить" /*начальные значения"*/, QIcon(":/icons/cancel.svg") }, [this]() { ResetCor(); } },         //
        { { "Задать" /*начальные значения"*/, QIcon(":/icons/Check-02-wf.svg") }, [this]() { WriteCor(); } },      //
        { { "Прочитать" /*значения из файла"*/, QIcon(":/icons/open.svg") }, [this]() { ReadFromFile(); } },       //
        { { "Сохранить" /*значения в файл"*/, QIcon(":/icons/save.svg") }, [this]() { SaveToFile(); } }            //
    };

    for (auto &i : funcs)
    {
        const QIcon &icon = i.first.second;
        QPushButton *pb = new QPushButton(icon, i.first.first);
        pb->setMinimumHeight(40);
        pb->setIconSize(QSize(pb->minimumHeight() / 2, pb->minimumHeight() / 2));

        connect(pb, &QAbstractButton::clicked, this, i.second);
        if (StdFunc::IsInEmulateMode())
            pb->setEnabled(false);
        hlyout->addWidget(pb);
    }
    lyout->addLayout(hlyout);
    w->setLayout(lyout);
    return w;
}

void AbstractStartupDialog::GetCorBd()
{
}

// void AbstractStartupDialog::ETHUpdate()
//{
//}

// void AbstractStartupDialog::MBSUpdate()
//{
//}

void AbstractStartupDialog::SetCor()
{
    iface()->writeCommand(Queries::QC_SetStartupValues);
    // if (MainInterface == I_ETHERNET)
    //{
    //    if (Board::GetInstance().interfaceType() == Board::InterfaceType::Ethernet)
    //        emit SendCom45(903);
    //}
    // else if (MainInterface == I_USB)
    //    else if (Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    //    {
    //        if (Commands::WriteCom(Commands::WriteStartupValues) == Error::Msg::NoError)
    //            QMessageBox::information(this, "INFO", "Записано успешно");
    //        else
    //            QMessageBox::information(this, "INFO", "Ошибка");
    //    }
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
    QList<DataTypes::SignalsStruct> list;
    //    DataManager::getSignals(m_startupBlockDescription.initStartRegAdr, m_startupBlockDescription.size / 4,
    //        DataTypes::SignalTypes::FloatWithTime, list); // /4 => float data by default
    if (list.isEmpty())
        return;
    for (const auto &signal : list)
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

void AbstractStartupDialog::updateFloatData(const DataTypes::FloatStruct &fl)
{
    if (!m_updatesEnabled)
        return;
    // Игнорируем 4011 т.к. он нам не важен и все чужие регистры тоже игнорируем
    if (fl.sigAdr >= m_regMap.firstKey() && fl.sigAdr < m_regMap.lastKey())
        FillBd(this, QString::number(fl.sigAdr), fl.sigVal);
}

void AbstractStartupDialog::updateStatus()
{
    DataTypes::GeneralResponseStruct grs;
    // while (DataManager::getResponse(DataTypes::GeneralResponseTypes::Ok, grs)
    //        != Error::Msg::ResEmpty) // get all responses from outList
    //        TimeFunc::Wait();
    // FIXME grs uninit
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
    {
        qDebug() << "Failed to find SpinBox with name:" << Name << "and parent:" << parent->objectName()
                 << "to setup value: " << Value;
    }
}

void AbstractStartupDialog::GetCorBdButton()
{
    iface()->reqStartup(m_startupBlockDescription.initStartRegAdr,
        m_startupBlockDescription.size / sizeof(float)); // /4 => float by default
}

bool AbstractStartupDialog::WriteCheckPassword()
{
    KeyPressDialog dlg; // = new KeyPressDialog;
    return dlg.CheckPassword("121941");
}

// void AbstractStartupDialog::TimerTimeout() { MessageTimer->stop(); }

void AbstractStartupDialog::ErrorRead()
{
    QMessageBox::information(this, "Ошибка", "Ошибка чтения");
}

void AbstractStartupDialog::uponInterfaceSetting()
{
    SetupUI();
}

void AbstractStartupDialog::reqUpdate()
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
            //            updateFloatData();
            m_updateState = AnswerWasReceived;
            break;
        }
        break;
        case AnswerWasReceived:
            //            FillCor();
            //            m_updateState = ThereWasNoUpdatesRecently;
            break;
        }
    }
    else
        m_updateState = ThereWasNoUpdatesRecently;
}
