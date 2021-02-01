#include "abstractconfdialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QTextEdit>

AbstractConfDialog::AbstractConfDialog(QWidget *parent) : UDialog(parent)
{
    m_password = "121941";
    setSuccessMsg("Конфигурация записана успешно");
    const auto &manager = DataManager::GetInstance();
    connect(&manager, &DataManager::confParametersListReceived, this, &AbstractConfDialog::confParametersListReceived);

    // connect(&manager, &DataManager::confParameterReceived, this, &AbstractConfDialog::confParameterReceived);
    // connect(&manager, &DataManager::responseReceived, this, &AbstractConfDialog::WriteConfMessageOk);
}

// void AbstractConfDialog::setConnections()
//{
//}

void AbstractConfDialog::ReadConf()
{
    //    iface()->reqFile(DataTypes::Config, true);
    BaseInterface::iface()->reqFile(DataTypes::Config, true);
    //    TimeFunc::Wait(100);
    //    switch (Board::GetInstance().interfaceType())
    //    {
    //    case Board::InterfaceType::Ethernet:
    //    {
    //        //        if ((ModuleBSI::Health() & HTH_CONFIG) || (StdFunc::IsInEmulateMode())) // если в модуле нет
    //        //        конфигурации,
    //        if (ModuleBSI::noConfig()) // если в модуле нет конфигурации,
    //                                   // заполнить поля по умолчанию
    //        {
    //            emit DefConfToBeLoaded();
    //        }
    //        else // иначе заполнить значениями из модуля
    //        {
    //            emit ReadConfig(1);
    //        }
    //        break;
    //    }
    //    case Board::InterfaceType::USB:
    //    {
    //        Error::Msg res = ModuleBSI::PrereadConf(this, S2::config);
    //        if (res == Error::Msg::ResEmpty)
    //            emit DefConfToBeLoaded();
    //        else if (res == Error::Msg::NoError)
    //            emit NewConfToBeLoaded();
    //        break;
    //    }
    //    default:
    //        break;
    //    }
}

// void AbstractConfDialog::FillConf(S2::configType *DR)
//{
//    S2::config = DR;
//    emit NewConfToBeLoaded();
//}

void AbstractConfDialog::WriteConf()
{

    if (!checkPassword())
        return;
    if (!PrepareConfToWrite())
    {
        qCritical("Ошибка чтения конфигурации");
        return;
    }
    //    iface()->writeConfigFile();
    BaseInterface::iface()->writeConfigFile();
    //        switch (Board::GetInstance().interfaceType())
    //        {
    //        case Board::InterfaceType::Ethernet:
    //            //            emit writeConfFile(S2::config);
    //            DataManager::setConfig(S2::config);
    //            break;
    //        case Board::InterfaceType::USB:
    //        {
    //            res = Commands::WriteFile(1, S2::config);
    //            if (res == Error::Msg::NoError)
    //            {
    //                emit BsiIsNeedToBeAcquiredAndChecked();
    //                QMessageBox::information(this, "Внимание", "Запись конфигурации и переход прошли успешно!");
    //            }
    //            else
    //                QMessageBox::critical(
    //                    this, "Ошибка", "Ошибка записи конфигурации" + QVariant::fromValue(res).toString());
    //            break;
    //        }
    //        default:
    //            break;
    //        }
}

void AbstractConfDialog::confParametersListReceived(const DataTypes::ConfParametersListStruct &cfpl)
{
    const auto &config = &S2::config;
    for (const auto &cfp : cfpl)
    {
        S2::findElemAndWriteIt(config, cfp);
    }
    Fill();
}

// void AbstractConfDialog::confParameterReceived(const DataTypes::ConfParameterStruct &cfp)
//{
//    // S2::findElemAndWriteIt(S2::config, cfp);
//}

// bool AbstractConfDialog::WriteCheckPassword()
//{
//    KeyPressDialog dlg; // = new KeyPressDialog;
//    return dlg.CheckPassword("121941");

//    ok = false;
//    StdFunc::ClearCancel();
//    QEventLoop PasswordLoop;
//    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
//    connect(dlg, &KeyPressDialog::Finished, this, &AbstractConfDialog::WritePasswordCheck);
//    connect(this, &AbstractConfDialog::WritePasswordChecked, &PasswordLoop, &QEventLoop::quit);
//    dlg->show();
//    PasswordLoop.exec();
//    if (StdFunc::IsCancelled())
//        return Error::Msg::GeneralError;
//    if (!ok)
//    {
//        QMessageBox::critical(this, "Неправильно", "Пароль введён неверно");
//        return Error::Msg::GeneralError;
//    }
//    return Error::Msg::NoError;
//}

// void AbstractConfDialog::WritePasswordCheck(QString psw)
//{
//    if (psw == "121941")
//        ok = true;
//    else
//        ok = false;
//    emit WritePasswordChecked();
//}

void AbstractConfDialog::SaveConfToFile()
{
    QByteArray ba;
    if (!PrepareConfToWrite())
    {
        qCritical("Ошибка чтения конфигурации");
        return;
    }
    S2::StoreDataMem(ba, &S2::config, DataTypes::Config);
    quint32 length = *reinterpret_cast<quint32 *>(&ba.data()[4]);
    length += sizeof(S2DataTypes::FileHeader);
    Q_ASSERT(length == quint32(ba.size()));

    Error::Msg res = Files::SaveToFile(Files::ChooseFileForSave(this, "Config files (*.cf)", "cf"), ba);
    switch (res)
    {
    case Error::Msg::NoError:
        QMessageBox::information(this, "Внимание", "Записано успешно!");
        break;
    case Error::Msg::FileWriteError:
        qCritical("Ошибка при записи файла!");
        break;
    case Error::Msg::FileNameError:
        qCritical("Пустое имя файла!");
        break;
    case Error::Msg::FileOpenError:
        qCritical("Ошибка открытия файла!");
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
        qCritical("Ошибка при загрузке файла конфигурации");
        return;
    }
    if (S2::RestoreDataMem(&(ba.data()[0]), ba.size(), &S2::config) != Error::Msg::NoError)
    {
        qCritical("Ошибка при разборе файла конфигурации");
        return;
    }
    //    emit NewConfToBeLoaded();
    Fill();
    QMessageBox::information(this, "Успешно", "Загрузка прошла успешно!");
}

QWidget *AbstractConfDialog::ConfButtons()
{
    QWidget *wdgt = new QWidget;
    QGridLayout *wdgtlyout = new QGridLayout;
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    QPushButton *pb = new QPushButton("Прочитать из " + tmps);
    connect(pb, &QAbstractButton::clicked, this, &AbstractConfDialog::ReadConf);
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
    connect(pb, &QAbstractButton::clicked, this, &AbstractConfDialog::SetDefConf);
    wdgtlyout->addWidget(pb, 2, 0, 1, 2);
    wdgt->setLayout(wdgtlyout);
    return wdgt;
}

// void AbstractConfDialog::ButtonReadConf()
//{
//    ReadConf();
//    switch (Board::GetInstance().interfaceType())
//    {
//    case Board::InterfaceType::Ethernet:
//    {
//        //        if ((ModuleBSI::Health() & HTH_CONFIG) || (StdFunc::IsInEmulateMode())) // если в модуле нет
//        //        конфигурации,
//        if ((ModuleBSI::noConfig()) || (StdFunc::IsInEmulateMode())) // если в модуле нет конфигурации,
//                                                                     // заполнить поля по умолчанию
//            emit DefConfToBeLoaded();
//        else // иначе заполнить значениями из модуля
//            emit ReadConfig(1);
//        break;
//    }
//    case Board::InterfaceType::USB:
//    {
//        Error::Msg res = ModuleBSI::PrereadConf(this, S2::config);
//        if (res == Error::Msg::ResEmpty)
//            emit DefConfToBeLoaded();
//        else if (res == Error::Msg::NoError)
//            emit NewConfToBeLoaded();
//        break;
//    }
//    }
//}

void AbstractConfDialog::PrereadConf()
{
    //    if ((ModuleBSI::Health() & HTH_CONFIG) || (StdFunc::IsInEmulateMode())) // если в модуле нет конфигурации,
    //    заполнить
    if (Board::GetInstance().noConfig()) // если в модуле нет конфигурации, заполнить поля по умолчанию
    {
        SetDefConf();
        QMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию", QMessageBox::Ok);
    }
    //        IsNeededDefConf = true; // emit LoadDefConf();
    else // иначе заполнить значениями из модуля
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
    CheckConf();
    if (CheckConfErrors.isEmpty())
        return true;

    QDialog *dlg = new QDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
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

void AbstractConfDialog::uponInterfaceSetting()
{
    SetupUI();
    PrereadConf();
}

void AbstractConfDialog::WriteConfMessageOk(const DataTypes::GeneralResponseStruct &rsp)
{
    if (rsp.type == DataTypes::GeneralResponseTypes::Ok)
        QMessageBox::information(this, "Внимание", "Запись конфигурации и переход прошли успешно!");
}

// void AbstractConfDialog::update()
//{
//    if (m_updatesEnabled)
//    {
//        switch (Board::GetInstance().interfaceType())
//        {
//        case Board::InterfaceType::USB:
//            USBUpdate();
//            break;
//        case Board::InterfaceType::Ethernet:
//            ETHUpdate();
//            break;
//        case Board::InterfaceType::RS485:
//            MBSUpdate();
//            break;
//        default:
//            break;
//        }
//    }
//}