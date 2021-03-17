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
    // connect(&manager, &DataManager::confParametersListReceived, this,
    // &AbstractConfDialog::confParametersListReceived);
    connect(&manager, &DataManager::dataRecVListReceived, this, &AbstractConfDialog::confReceived);
}

void AbstractConfDialog::ReadConf()
{
    BaseInterface::iface()->reqFile(DataTypes::Config, true);
}

void AbstractConfDialog::WriteConf()
{
    if (!checkPassword())
        return;
    if (!PrepareConfToWrite())
    {
        qCritical("Ошибка чтения конфигурации");
        return;
    }
    S2DataTypes::S2ConfigType buffer;
    const auto &confList = S2::configV;

    std::transform(confList.begin(), confList.end(), std::back_inserter(buffer),
        [](const auto &record) -> S2DataTypes::DataRec { return record.serialize(); });
    S2::tester(buffer);

    buffer.push_back({ S2DataTypes::dummyElement, 0, nullptr });
    BaseInterface::iface()->writeS2File(DataTypes::Config, &buffer);
}

void AbstractConfDialog::confReceived(const QList<DataTypes::DataRecV> &list)
{
    //  S2::configV.clear();
    //   const auto &configV = &S2::configV;
    S2::configV = list;
    Fill();
    // std::swap(list,configV);
    // std::swap(S2::configV,list);
}

void AbstractConfDialog::confParametersListReceived(const DataTypes::ConfParametersListStruct &cfpl)
{
    const auto &config = &S2::config;
    S2::configV.clear();
    const auto &configV = &S2::configV;
    for (const auto &cfp : cfpl)
    {
        S2::findElemAndWriteIt(config, cfp);
    }
    std::transform(config->begin(), config->end(), std::back_inserter(S2::configV),
        [](const auto &oldRec) -> DataTypes::DataRecV { return DataTypes::DataRecV(oldRec); });
    // std::for_each(configV->cbegin(), configV->cend(), [](auto &value) { value.printer(); });
    Fill();
    //    qDebug() << std::equal(config->cbegin(), config->cend(), configV->cbegin(), configV->cend(),
    //        [](const S2DataTypes::DataRec &oldRec, const S2DataTypes::DataRecV &newRec) {
    //            bool status = true;
    //            status = S2DataTypes::is_same(oldRec, newRec.serialize());
    //            if (!status)
    //                qDebug() << status;
    //            return status;
    //        });
    for (auto i = 0; i != configV->size() && i != config->size(); ++i)
    {
        const auto oldRec = config->at(i);
        const auto newRec = configV->at(i).serialize();
        if (!S2DataTypes::is_same(oldRec, newRec))
            qDebug() << oldRec.id << oldRec.numByte;
    }
}

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
    if (!S2::RestoreDataMem(&(ba.data()[0]), ba.size(), &S2::config))
    {
        qCritical("Ошибка при разборе файла конфигурации");
        return;
    }
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
    pb->setIcon(QIcon(":/icons/tnload.svg"));
    connect(pb, &QAbstractButton::clicked, this, &AbstractConfDialog::LoadConfFromFile);
    wdgtlyout->addWidget(pb, 1, 0, 1, 1);
    pb = new QPushButton("Записать в файл");
    pb->setIcon(QIcon(":/icons/tnsave.svg"));
    connect(pb, &QAbstractButton::clicked, this, &AbstractConfDialog::SaveConfToFile);
    wdgtlyout->addWidget(pb, 1, 1, 1, 1);
    pb = new QPushButton("Взять конфигурацию по умолчанию");
    connect(pb, &QAbstractButton::clicked, this, [this] { SetDefConf(); });
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
