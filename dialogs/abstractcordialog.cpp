#include "abstractcordialog.h"
#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStringListModel>
#include <QSpinBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QTabBar>
#include <QFileDialog>
#include "../gen/error.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/files.h"
#include "../widgets/etableview.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../gen/maindef.h"
#include "../gen/colors.h"
#include "../gen/timefunc.h"
#include "../dialogs/keypressdialog.h"
#include "../usb/commands.h"

AbstractCorDialog::AbstractCorDialog(QWidget *parent) :
    QDialog(parent)
{

}

void AbstractCorDialog::GetCorBd(int index)
{
   Q_UNUSED(index);
}

void AbstractCorDialog::SetCor()
{
    if (MainInterface == I_ETHERNET)
    {
       emit SendCom45(903);
    }
    else if (MainInterface == I_USB)
    {
       if(Commands::WriteCom(4) == NOERROR)
           EMessageBox::information(this, "INFO", "Записано успешно");
       else
           EMessageBox::information(this, "INFO", "Ошибка");
    }
}


float AbstractCorDialog::ToFloat(QString text)
{
    bool ok;
    float tmpf;
    tmpf = text.toFloat(&ok);
    if (!ok)
    {
        ERMSG("Значение "+text+" не может быть переведено во float");
        return 0;
    }
    return tmpf;
}

void AbstractCorDialog::MessageOk()
{
  EMessageBox::information(this, "INFO", "Записано успешно");
}

void AbstractCorDialog::UpdateFlCorData(IEC104Thread::FlSignals104 *Signal)
{
    IEC104Thread::FlSignals104 sig = *new IEC104Thread::FlSignals104;
    int i;

    if(((Signal)->fl.SigAdr >= 4000) && ((Signal)->fl.SigAdr <= 4010))
    {
        for(i=0; i<Signal->SigNumber; i++)
        {
            sig = *(Signal+i);
            FillBd(this, QString::number((Signal+i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal+i)->fl.SigVal));
        }

        if(first)
        EMessageBox::information(this, "INFO", "Прочитано успешно");
        else
        first = 1;
    }

}


void AbstractCorDialog::FillBd(QWidget *parent, QString Name, QString Value)
{
    float fl;
    QLocale german(QLocale::German);
    fl = german.toDouble(Value);
    WDFunc::SetSPBData(parent, Name, fl);
}

void AbstractCorDialog::ModBusUpdateCorData(QList<ModBus::SignalStruct> Signal)
{
    int i = 0;

    if (Signal.size() > 0)
    {
        if(Signal.at(0).SigAdr == 4000)
        {
            for(i=0; i<Signal.size(); ++i)
            {
                FillBd(this, QString::number(Signal.at(i).SigAdr), WDFunc::StringValueWithCheck(Signal.at(i).flVal));
            }
            EMessageBox::information(this, "INFO", "Прочитано успешно");
        }
    }
}

int AbstractCorDialog::WriteCheckPassword()
{
    ok = false;
    StdFunc::ClearCancel();
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    connect(dlg,SIGNAL(Finished(QString)),this,SLOT(WritePasswordCheck(QString)));
    connect(this,SIGNAL(WritePasswordChecked()),&PasswordLoop,SLOT(quit()));
    dlg->deleteLater();
    dlg->show();
    PasswordLoop.exec();
    if (StdFunc::IsCancelled())
        return GENERALERROR;
    if (!ok)
    {
        EMessageBox::error(this, "Неправильно", "Пароль введён неверно");
        return GENERALERROR;
    }
    return NOERROR;
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
  EMessageBox::information(this, "Ошибка", "Ошибка чтения");
}

