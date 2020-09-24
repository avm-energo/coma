#include "abstractcordialog.h"

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

AbstractCorDialog::AbstractCorDialog(QWidget *parent) : QDialog(parent)
{
}

void AbstractCorDialog::GetCorBd(int index)
{
    Q_UNUSED(index);
}

void AbstractCorDialog::SetCor()
{
    // if (MainInterface == I_ETHERNET)
    //{
    if (Board::GetInstance()->interfaceType() == Board::InterfaceType::Ethernet)
        emit SendCom45(903);
    //}
    // else if (MainInterface == I_USB)
    else if (Board::GetInstance()->interfaceType() == Board::InterfaceType::USB)
        //   {
        if (Commands::WriteCom(4) == Error::Msg::NoError)
            QMessageBox::information(this, "INFO", "Записано успешно");
        else
            QMessageBox::information(this, "INFO", "Ошибка");
    //    }
}

float AbstractCorDialog::ToFloat(QString text)
{
    bool ok;
    float tmpf;
    tmpf = text.toFloat(&ok);
    if (!ok)
    {
        ERMSG("Значение " + text + " не может быть переведено во float");
        return 0;
    }
    return tmpf;
}

void AbstractCorDialog::MessageOk()
{
    QMessageBox::information(this, "INFO", "Записано успешно");
}

void AbstractCorDialog::UpdateFlCorData(IEC104Thread::FlSignals104 *Signal)
{

    if (((Signal)->fl.SigAdr >= 4000) && ((Signal)->fl.SigAdr <= 4010))
    {
        for (int i = 0; i < Signal->SigNumber; i++)
        {
            FillBd(
                this, QString::number((Signal + i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal + i)->fl.SigVal));
        }

        if (first)
            QMessageBox::information(this, "INFO", "Прочитано успешно");
        else
            first = 1;
    }
}

void AbstractCorDialog::FillBd(QWidget *parent, QString Name, QString Value)
{
    WDFunc::SetSPBData(parent, Name, Value.toDouble());
}

void AbstractCorDialog::ModBusUpdateCorData(QList<ModBus::SignalStruct> Signal)
{
    int i = 0;

    if (Signal.size() > 0)
    {
        if (Signal.at(0).SigAdr == 4000)
        {
            for (i = 0; i < Signal.size(); ++i)
            {
                FillBd(this, QString::number(Signal.at(i).SigAdr), WDFunc::StringValueWithCheck(Signal.at(i).flVal));
            }
            QMessageBox::information(this, "INFO", "Прочитано успешно");
        }
    }
}

Error::Msg AbstractCorDialog::WriteCheckPassword()
{
    ok = false;
    StdFunc::ClearCancel();
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    connect(dlg, SIGNAL(Finished(QString)), this, SLOT(WritePasswordCheck(QString)));
    connect(this, SIGNAL(WritePasswordChecked()), &PasswordLoop, SLOT(quit()));
    dlg->deleteLater();
    dlg->show();
    PasswordLoop.exec();
    if (StdFunc::IsCancelled())
        return Error::Msg::GeneralError;
    if (!ok)
    {
        QMessageBox::critical(this, "Неправильно", "Пароль введён неверно");
        return Error::Msg::GeneralError;
    }
    return Error::Msg::NoError;
}
void AbstractCorDialog::WritePasswordCheck(QString psw)
{
    if (psw == "121941")
        ok = true;
    else
        ok = false;
    emit WritePasswordChecked();
}

void AbstractCorDialog::TimerTimeout()
{
    MessageTimer->stop();
}

void AbstractCorDialog::ErrorRead()
{
    QMessageBox::information(this, "Ошибка", "Ошибка чтения");
}
