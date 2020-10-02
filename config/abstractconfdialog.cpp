#include "abstractconfdialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../iec104/iec104.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QTextEdit>

AbstractConfDialog::AbstractConfDialog(QWidget *parent) : QDialog(parent)
{
}

void AbstractConfDialog::ReadConf()
{

    TimeFunc::Wait(100);
    switch (Board::GetInstance()->interfaceType())
    {
    case Board::InterfaceType::Ethernet:
    {
        if ((ModuleBSI::Health() & HTH_CONFIG) || (StdFunc::IsInEmulateMode())) // если в модуле нет конфигурации,
                                                                                // заполнить поля по умолчанию
        {
            emit DefConfToBeLoaded();
        }
        else // иначе заполнить значениями из модуля
        {
            emit ReadConfig(1);
        }
        break;
    }
    case Board::InterfaceType::USB:
    {
        Error::Msg res = ModuleBSI::PrereadConf(this, S2Config);
        if (res == Error::Msg::ResEmpty)
            emit DefConfToBeLoaded();
        else if (res == Error::Msg::NoError)
            emit NewConfToBeLoaded();
        break;
    }
    default:
        break;
    }
}

void AbstractConfDialog::FillConf(QVector<S2::DataRec> *DR)
{
    S2Config = DR;
    emit NewConfToBeLoaded();
}

void AbstractConfDialog::WriteConf()
{
    Error::Msg res = WriteCheckPassword();
    if (res == Error::Msg::NoError)
    {
        if (!PrepareConfToWrite())
        {
            ERMSG("Ошибка чтения конфигурации");
            return;
        }
        switch (Board::GetInstance()->interfaceType())
        {
        case Board::InterfaceType::Ethernet:
            emit writeConfFile(S2Config);
            break;
        case Board::InterfaceType::USB:
        {
            res = Commands::WriteFile(1, S2Config);
            if (res == Error::Msg::NoError)
            {
                emit BsiIsNeedToBeAcquiredAndChecked();
                QMessageBox::information(this, "Внимание", "Запись конфигурации и переход прошли успешно!");
            }
            else
                QMessageBox::critical(
                    this, "Ошибка", "Ошибка записи конфигурации" + QVariant::fromValue(res).toString());
            break;
        }
        }
    }
}

Error::Msg AbstractConfDialog::WriteCheckPassword()
{
    ok = false;
    StdFunc::ClearCancel();
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    connect(dlg, &KeyPressDialog::Finished, this, &AbstractConfDialog::WritePasswordCheck);
    connect(this, &AbstractConfDialog::WritePasswordChecked, &PasswordLoop, &QEventLoop::quit);
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

void AbstractConfDialog::WritePasswordCheck(QString psw)
{
    if (psw == "121941")
        ok = true;
    else
        ok = false;
    emit WritePasswordChecked();
}

void AbstractConfDialog::SaveConfToFile()
{
    QByteArray ba;
    if (!PrepareConfToWrite())
    {
        ERMSG("Ошибка чтения конфигурации");
        return;
    }
    ba.resize(MAXBYTEARRAY);
    S2::StoreDataMem(&(ba.data()[0]), S2Config,
        0x0001); // 0x0001 - номер файла конфигурации
    quint32 BaLength = static_cast<quint8>(ba.data()[4]);
    BaLength += static_cast<quint8>(ba.data()[5]) * 256;
    BaLength += static_cast<quint8>(ba.data()[6]) * 65536;
    BaLength += static_cast<quint8>(ba.data()[7]) * 16777216;
    BaLength += sizeof(S2::FileHeader); // FileHeader
    Error::Msg res = Files::SaveToFile(Files::ChooseFileForSave(this, "Config files (*.cf)", "cf"), ba, BaLength);
    switch (res)
    {
    case Error::Msg::NoError:
        QMessageBox::information(this, "Внимание", "Записано успешно!");
        break;
    case Error::Msg::FILE_WRITE:
        ERMSG("Ошибка при записи файла!");
        break;
    case Error::Msg::FILE_NAMEEMP:
        ERMSG("Пустое имя файла!");
        break;
    case Error::Msg::FILE_OPEN:
        ERMSG("Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void AbstractConfDialog::LoadConfFromFile()
{
    QByteArray ba;
    Error::Msg res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "Config files (*.cf)"), ba);
    if (res != Error::Msg::NoError)
    {
        WARNMSG("Ошибка при загрузке файла конфигурации");
        return;
    }
    if (S2::RestoreDataMem(&(ba.data()[0]), ba.size(), S2Config) != Error::Msg::NoError)
    {
        WARNMSG("Ошибка при разборе файла конфигурации");
        return;
    }
    emit NewConfToBeLoaded();
    QMessageBox::information(this, "Успешно", "Загрузка прошла успешно!");
}

QWidget *AbstractConfDialog::ConfButtons()
{
    QWidget *wdgt = new QWidget;
    QGridLayout *wdgtlyout = new QGridLayout;
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    QPushButton *pb = new QPushButton("Прочитать из " + tmps);
    connect(pb, &QAbstractButton::clicked, this, &AbstractConfDialog::ButtonReadConf);
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 0, 0, 1, 1);
    tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    pb = new QPushButton("Записать в " + tmps);
    pb->setObjectName("WriteConfPB");
    connect(pb, &QAbstractButton::clicked, this, &AbstractConfDialog::WriteConf);
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 0, 1, 1, 1);
    pb = new QPushButton("Прочитать из файла");
    pb->setIcon(QIcon("images/load.png"));
    connect(pb, &QAbstractButton::clicked, this, &AbstractConfDialog::LoadConfFromFile);
    wdgtlyout->addWidget(pb, 1, 0, 1, 1);
    pb = new QPushButton("Записать в файл");
    pb->setIcon(QIcon("images/save.png"));
    connect(pb, &QAbstractButton::clicked, this, &AbstractConfDialog::SaveConfToFile);
    wdgtlyout->addWidget(pb, 1, 1, 1, 1);
    pb = new QPushButton("Взять конфигурацию по умолчанию");
    connect(pb, &QAbstractButton::clicked, this, &AbstractConfDialog::DefConfToBeLoaded);
    wdgtlyout->addWidget(pb, 2, 0, 1, 2);
    wdgt->setLayout(wdgtlyout);
    return wdgt;
}

void AbstractConfDialog::ButtonReadConf()
{
    /*    char* num = new char;
     *num = 1; */
    switch (Board::GetInstance()->interfaceType())
    {
    case Board::InterfaceType::Ethernet:
    {
        if ((ModuleBSI::Health() & HTH_CONFIG) || (StdFunc::IsInEmulateMode())) // если в модуле нет конфигурации,
                                                                                // заполнить поля по умолчанию
            emit DefConfToBeLoaded();
        else // иначе заполнить значениями из модуля
            emit ReadConfig(1);
        break;
    }
    case Board::InterfaceType::USB:
    {
        Error::Msg res = ModuleBSI::PrereadConf(this, S2Config);
        if (res == Error::Msg::ResEmpty)
            emit DefConfToBeLoaded();
        else if (res == Error::Msg::NoError)
            emit NewConfToBeLoaded();
        break;
    }
    }
}

void AbstractConfDialog::PrereadConf()
{
    if ((ModuleBSI::Health() & HTH_CONFIG) || (StdFunc::IsInEmulateMode())) // если в модуле нет конфигурации, заполнить
                                                                            // поля по умолчанию
        IsNeededDefConf = true; // emit LoadDefConf();
    else                        // иначе заполнить значениями из модуля
                                //        ReadConf(confIndex);
        ReadConf();
}

// по имени виджета взять его номер

// int AbstractConfDialog::GetChNumFromObjectName(QString ObjectName)
//{
//    QStringList ObjectNameSl = ObjectName.split(".");
//    int ChNum;
//    bool ok;
//    if (ObjectNameSl.size() > 1)
//        ChNum = ObjectNameSl.at(1).toInt(&ok);
//    else
//        return Error::Msg::GeneralError;
//    if (!ok)
//        return Error::Msg::GeneralError;
//    return ChNum;
//}

bool AbstractConfDialog::PrepareConfToWrite()
{
    FillBack();
    CheckConfErrors.clear();
    // CheckConf();
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
        connect(pb, &QAbstractButton::clicked, dlg, &QWidget::close);
        vlyout->addWidget(pb);
        dlg->setLayout(vlyout);
        dlg->show();
        return false;
    }
    return true;
}

void AbstractConfDialog::WriteConfMessageOk()
{
    QMessageBox::information(this, "Внимание", "Запись конфигурации и переход прошли успешно!");
}
