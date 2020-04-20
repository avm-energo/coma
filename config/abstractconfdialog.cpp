#include <QPushButton>
#include <QCoreApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <QFileDialog>
#include "abstractconfdialog.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
//#include "../canal.h"
#include "../gen/maindef.h"
#include "../gen/stdfunc.h"
#include "../gen/files.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../iec104/iec104.h"
#include "../gen/mainwindow.h"
#include "../gen/timefunc.h"
#include "../dialogs/keypressdialog.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

AbstractConfDialog::AbstractConfDialog(QWidget *parent) : QDialog(parent)
{
}

#if PROGSIZE != PROGSIZE_EMUL
void AbstractConfDialog::ReadConf(int index)
{
    //int res = ModuleBSI::PrereadConf(this, S2Config);
    //int res = 0;
    char* num = new char;
    *num = 1;

    if(index == 1)
    {
        if(timeIndex)
        emit stopRead(timeIndex);

        if(checkIndex)
        emit stopRead(checkIndex);

        TimeFunc::Wait(100);

        if(MainWindow::MainInterface.size() != 0)
        {
         if(MainWindow::MainInterface == "Ethernet")
         {
            if ((ModuleBSI::Health() & HTH_CONFIG) || (StdFunc::IsInEmulateMode())) // если в модуле нет конфигурации, заполнить поля по умолчанию
            {
              emit DefConfToBeLoaded();
            }
            else // иначе заполнить значениями из модуля
            {
              emit ReadConfig(num);
            }
         }
         else if(MainWindow::MainInterface == "USB")
         {
             int res = ModuleBSI::PrereadConf(this, S2Config);
             if (res == Error::ER_RESEMPTY)
                 emit DefConfToBeLoaded();
             else if (res == Error::ER_NOERROR)
                 emit NewConfToBeLoaded();

         }
        }
    }



}

void AbstractConfDialog::FillConf(QVector<S2::DataRec>* DR)
{
   S2Config = DR;
   emit NewConfToBeLoaded();
}

void AbstractConfDialog::WriteConf()
{
    int res;
    if (WriteCheckPassword() == Error::ER_NOERROR)
    {

        if (!PrepareConfToWrite())
            return;

        if(MainWindow::MainInterface.size() != 0)
        {
         if(MainWindow::MainInterface == "Ethernet")
         {
           emit writeConfFile(S2Config);
         }
         else if(MainWindow::MainInterface == "USB")
         {
            if ((res = Commands::WriteFile(nullptr, 1, S2Config)) == Error::ER_NOERROR)
            {
                emit BsiIsNeedToBeAcquiredAndChecked();
                EMessageBox::information(this, "Внимание", "Запись конфигурации и переход прошли успешно!");
            }
            else
                EMessageBox::error(this, "Ошибка", "Ошибка записи конфигурации"+QString::number(res));
         }
        }
    }
}

int AbstractConfDialog::WriteCheckPassword()
{
    ok = false;
    StdFunc::ClearCancel();
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    connect(dlg,SIGNAL(Finished(QString)),this,SLOT(WritePasswordCheck(QString)));
    connect(this,SIGNAL(WritePasswordChecked()),&PasswordLoop,SLOT(quit()));
    dlg->show();
    PasswordLoop.exec();
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    if (!ok)
    {
        EMessageBox::error(this, "Неправильно", "Пароль введён неверно");
        return Error::ER_GENERALERROR;
    }
    return Error::ER_NOERROR;
}

void AbstractConfDialog::WritePasswordCheck(QString psw)
{
    if (psw == "121941")
        ok = true;
    else
        ok = false;
    emit WritePasswordChecked();
}

#endif
void AbstractConfDialog::SaveConfToFile()
{
    QByteArray ba;
    if (!PrepareConfToWrite())
        return;
    ba.resize(MAXBYTEARRAY);
    S2::StoreDataMem(&(ba.data()[0]), S2Config, 0x0001); // 0x0001 - номер файла конфигурации
    quint32 BaLength = static_cast<quint8>(ba.data()[4]);
    BaLength += static_cast<quint8>(ba.data()[5])*256;
    BaLength += static_cast<quint8>(ba.data()[6])*65536;
    BaLength += static_cast<quint8>(ba.data()[7])*16777216;
    BaLength += sizeof(S2::FileHeader); // FileHeader
    int res = Files::SaveToFile(Files::ChooseFileForSave(this, "Config files (*.cf)", "cf"), ba, BaLength);
    switch (res)
    {
    case Files::ER_NOERROR:
        EMessageBox::information(this, "Внимание", "Записано успешно!");
        break;
    case Files::ER_FILEWRITE:
        ERMSG("Ошибка при записи файла!");
        break;
    case Files::ER_FILENAMEEMP:
        ERMSG("Пустое имя файла!");
        break;
    case Files::ER_FILEOPEN:
        ERMSG("Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void AbstractConfDialog::LoadConfFromFile()
{
    QByteArray ba;
    int res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "Config files (*.cf)"), ba);
    if (res != Files::ER_NOERROR)
    {
        WARNMSG("Ошибка при загрузке файла конфигурации");
        return;
    }
    if (S2::RestoreDataMem(&(ba.data()[0]), ba.size(), S2Config) != Error::ER_NOERROR)
    {
        WARNMSG("Ошибка при разборе файла конфигурации");
        return;
    }
    emit NewConfToBeLoaded();
    EMessageBox::information(this, "Успешно", "Загрузка прошла успешно!");
}

QWidget *AbstractConfDialog::ConfButtons()
{
    QWidget *wdgt = new QWidget;
    QGridLayout *wdgtlyout = new QGridLayout;
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    QPushButton *pb = new QPushButton("Прочитать из " + tmps);
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(ButtonReadConf()));
#endif
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 0, 0, 1, 1);
    tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    pb = new QPushButton("Записать в " + tmps);
    pb->setObjectName("WriteConfPB");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteConf()));
#endif
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 0, 1, 1, 1);
    pb = new QPushButton("Прочитать из файла");
    pb->setIcon(QIcon("images/load.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadConfFromFile()));
    wdgtlyout->addWidget(pb, 1, 0, 1, 1);
    pb = new QPushButton("Записать в файл");
    pb->setIcon(QIcon("images/save.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveConfToFile()));
    wdgtlyout->addWidget(pb, 1, 1, 1, 1);
    pb = new QPushButton("Задать конфигурацию по умолчанию");
    connect(pb,SIGNAL(clicked()),this,SIGNAL(DefConfToBeLoaded()));
    wdgtlyout->addWidget(pb, 2, 0, 1, 2);
    wdgt->setLayout(wdgtlyout);
    return wdgt;
}

void AbstractConfDialog::ButtonReadConf()
{
    char* num = new char;
    *num = 1;

    if(MainWindow::MainInterface.size() != 0)
    {
     if(MainWindow::MainInterface == "Ethernet")
     {
        if ((ModuleBSI::Health() & HTH_CONFIG) || (StdFunc::IsInEmulateMode())) // если в модуле нет конфигурации, заполнить поля по умолчанию
        {
          emit DefConfToBeLoaded();
        }
        else // иначе заполнить значениями из модуля
        {
          emit ReadConfig(num);
        }
     }
     else if(MainWindow::MainInterface == "USB")
     {
         int res = ModuleBSI::PrereadConf(this, S2Config);
         if (res == Error::ER_RESEMPTY)
             emit DefConfToBeLoaded();
         else if (res == Error::ER_NOERROR)
             emit NewConfToBeLoaded();

     }
    }
}

#if PROGSIZE != PROGSIZE_EMUL
void AbstractConfDialog::PrereadConf()
{
    if ((ModuleBSI::Health() & HTH_CONFIG) || (StdFunc::IsInEmulateMode())) // если в модуле нет конфигурации, заполнить поля по умолчанию
     IsNeededDefConf = true;  // emit LoadDefConf();
    else // иначе заполнить значениями из модуля
     ReadConf(confIndex);
}
#endif
// по имени виджета взять его номер

int AbstractConfDialog::GetChNumFromObjectName(QString ObjectName)
{
    QStringList ObjectNameSl = ObjectName.split(".");
    int ChNum;
    bool ok;
    if (ObjectNameSl.size()>1)
        ChNum = ObjectNameSl.at(1).toInt(&ok);
    else
        return Error::ER_GENERALERROR;
    if (!ok)
        return Error::ER_GENERALERROR;
    return ChNum;
}

bool AbstractConfDialog::PrepareConfToWrite()
{
    FillBack();
    CheckConfErrors.clear();
    //CheckConf();
    if (!CheckConfErrors.isEmpty())
    {
        QDialog *dlg = new QDialog;
        QVBoxLayout *vlyout = new QVBoxLayout;
        QLabel *lbl = new QLabel("В конфигурации есть ошибки, проверьте и исправьте");
        vlyout->addWidget(lbl, 0, Qt::AlignLeft);
        QTextEdit *te = new QTextEdit;
        te->setPlainText(CheckConfErrors.join("\n"));
        vlyout->addWidget(te, 0, Qt::AlignCenter);
        QPushButton *pb = new QPushButton("Хорошо");
        connect(pb,SIGNAL(clicked(bool)),dlg,SLOT(close()));
        vlyout->addWidget(pb);
        dlg->setLayout(vlyout);
        dlg->exec();
        return false;
    }
    return true;
}
