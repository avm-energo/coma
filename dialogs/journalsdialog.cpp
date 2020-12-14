#include "../dialogs/journalsdialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../widgets/wd_func.h"
#ifdef MODELDEBUG
#include <QAbstractItemModelTester>
#endif
#include <QDateTime>
#include <QDebug>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

JournalDialog::JournalDialog(Journals *jour, QWidget *parent) : UDialog(parent)
{
    JourFuncs = jour;
    connect(&DataManager::GetInstance(), &DataManager::fileReceived, JourFuncs, &Journals::FillJour);
    ProxyWorkModel = new QSortFilterProxyModel(this);
    ProxySysModel = new QSortFilterProxyModel(this);
    ProxyMeasModel = new QSortFilterProxyModel(this);
    progress = new QProgressDialog(this);
    progress->setCancelButton(nullptr);
    progress->cancel();

    JourFuncs->SetProxyModels(ProxyWorkModel, ProxySysModel, ProxyMeasModel);

    connect(JourFuncs, &Journals::Done, this, &JournalDialog::Done);
    connect(JourFuncs, &Journals::Error, this, &JournalDialog::Error);
    // connect(JourFuncs, &Journals::ModelReady, this, &JournalDialog::SetModel);
    //    connect(JourFuncs,SIGNAL(ProxyModelReady(QSortFilterProxyModel
    //    *)),this,SLOT(SetProxyModel(QSortFilterProxyModel *)));
    //    if (Board::GetInstance().interfaceType() == Board::InterfaceType::Ethernet)
    //    {
    //        connect(JourFuncs, &Journals::ReadJour, iec, &IEC104::SelectFile);
    //        connect(iec, &IEC104::SendJourSysfromiec104, JourFuncs, &Journals::FillSysJour);
    //        connect(iec, &IEC104::SendJourWorkfromiec104, JourFuncs, &Journals::FillWorkJour);
    //        connect(iec, &IEC104::SendJourMeasfromiec104, JourFuncs, &Journals::FillMeasJour);
    //    }
    //    connect(this, &JournalDialog::StartGetJour, JourFuncs, &Journals::StartGetJour);
    //    connect(this, &JournalDialog::StartSaveJour, JourFuncs, &Journals::StartSaveJour);
    //    connect(this, &JournalDialog::StartReadFile, JourFuncs, &Journals::ReadJourFileAndProcessIt);
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

JournalDialog::~JournalDialog()
{
}

void JournalDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    /*    QVBoxLayout *vlyout = new QVBoxLayout;
        QHBoxLayout *hlyout = new QHBoxLayout; */
    // QString tmps = "QDialog {background-color: " + QString(Colors::ACONFCLR) + ";}";
    // setStyleSheet(tmps);
    // QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(Colors::TABCOLOR) + ";}";

    /*    QTabWidget *work = new QTabWidget;
        work->tabBar()->setStyleSheet(ConfTWss);
        hlyout->addWidget(WDFunc::NewPB(this, "gj." +
       QString::number(Files::JourWork), "Получить рабочий журнал", this,
       SLOT(TryGetJourByUSB()))); hlyout->addWidget(WDFunc::NewPB(this, "ej." +
       QString::number(Files::JourWork), "Стереть рабочий журнал", this,
       SLOT(EraseJour()))); hlyout->addWidget(WDFunc::NewPB(this, "sj." +
       QString::number(Files::JourWork), "Сохранить журнал в файл", this,
       SLOT(SaveJour()))); vlyout->addLayout(hlyout);
        vlyout->addWidget(WDFunc::NewTV(this, "work", nullptr), 89);
        work->setLayout(vlyout);
        work->setStyleSheet(ConfTWss);

        QTabWidget *system = new QTabWidget;
        system->tabBar()->setStyleSheet(ConfTWss);

        hlyout = new QHBoxLayout;
        vlyout = new QVBoxLayout;
        hlyout->addWidget(WDFunc::NewPB(this, "gj." +
       QString::number(Files::JourSys), "Получить системный журнал", this,
       SLOT(TryGetJourByUSB()))); hlyout->addWidget(WDFunc::NewPB(this, "ej." +
       QString::number(Files::JourSys), "Стереть системный журнал", this,
       SLOT(EraseJour()))); hlyout->addWidget(WDFunc::NewPB(this, "sj." +
       QString::number(Files::JourSys), "Сохранить журнал в файл", this,
       SLOT(SaveJour()))); vlyout->addLayout(hlyout);
        vlyout->addWidget(WDFunc::NewTV(this, "system", nullptr), 89);
        system->setLayout(vlyout);
        system->setStyleSheet(ConfTWss);

        QTabWidget *measure = new QTabWidget;
        measure->tabBar()->setStyleSheet(ConfTWss);
        hlyout = new QHBoxLayout;
        vlyout = new QVBoxLayout;
        hlyout->addWidget(WDFunc::NewPB(this, "gj." +
       QString::number(Files::JourMeas), "Получить журнал измерений", this,
       SLOT(TryGetJourByUSB()))); hlyout->addWidget(WDFunc::NewPB(this, "ej." +
       QString::number(Files::JourMeas), "Стереть журнал измерений", this,
       SLOT(EraseJour()))); hlyout->addWidget(WDFunc::NewPB(this, "sj." +
       QString::number(Files::JourMeas), "Сохранить журнал в файл", this,
       SLOT(SaveJour()))); vlyout->addLayout(hlyout);
        vlyout->addWidget(WDFunc::NewTV(this, "meas", nullptr), 89);
        measure->setLayout(vlyout);
        measure->setStyleSheet(ConfTWss);
    */
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw4");
    // ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(JourTab(DataTypes::JourWork), "Рабочий журнал");
    ConfTW->addTab(JourTab(DataTypes::JourSys), "Системный журнал");
    ConfTW->addTab(JourTab(DataTypes::JourMeas), "Журнал измерений");

    lyout->addWidget(ConfTW);
    setLayout(lyout);
    if (StdFunc::IsInEmulateMode())
    {
        WDFunc::SetEnabled(this, "gj." + QString::number(DataTypes::JourSys), false);
        WDFunc::SetEnabled(this, "gj." + QString::number(DataTypes::JourMeas), false);
        WDFunc::SetEnabled(this, "gj." + QString::number(DataTypes::JourWork), false);
        WDFunc::SetEnabled(this, "ej." + QString::number(DataTypes::JourSys), false);
        WDFunc::SetEnabled(this, "ej." + QString::number(DataTypes::JourMeas), false);
        WDFunc::SetEnabled(this, "ej." + QString::number(DataTypes::JourWork), false);
        WDFunc::SetEnabled(this, "sj." + QString::number(DataTypes::JourSys), false);
        WDFunc::SetEnabled(this, "sj." + QString::number(DataTypes::JourMeas), false);
        WDFunc::SetEnabled(this, "sj." + QString::number(DataTypes::JourWork), false);
    }
}

QWidget *JournalDialog::JourTab(int jourtype)
{
    QSortFilterProxyModel *mdl;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QWidget *w = new QWidget;
    QString str, tvname;

    switch (jourtype)
    {
    case DataTypes::JourWork:
        str = "рабочий журнал";
        tvname = "work";
        mdl = ProxyWorkModel;
        break;
    case DataTypes::JourSys:
        str = "системный журнал";
        tvname = "system";
        mdl = ProxySysModel;
        break;
    case DataTypes::JourMeas:
        str = "журнал измерений";
        tvname = "meas";
        mdl = ProxyMeasModel;
        break;
    default:
        qDebug("Default case");
        return w;
        break;
    }
    QPushButton *getButton = WDFunc::NewPB(
        this, "gj." + QString::number(jourtype), "Получить " + str, this, &JournalDialog::TryGetJourByUSB);
    hlyout->addWidget(getButton);
    QPushButton *eraseButton
        = WDFunc::NewPB(this, "ej." + QString::number(jourtype), "Стереть " + str, this, &JournalDialog::EraseJour);
    hlyout->addWidget(eraseButton);
    QPushButton *saveButton = WDFunc::NewPB(
        this, "sj." + QString::number(jourtype), "Сохранить журнал в файл", this, &JournalDialog::SaveJour);
    saveButton->setEnabled(false);
    connect(JourFuncs, &Journals::Done, [saveButton](const QString &str, const int &number) {
        Q_UNUSED(str)
        if (saveButton->objectName().back().digitValue() == number)
            saveButton->setEnabled(true);
    });
    hlyout->addWidget(saveButton);
    vlyout->addLayout(hlyout);
    auto *modelView = WDFunc::NewTV(this, tvname, mdl);
    vlyout->addWidget(modelView, 89);
    w->setLayout(vlyout);
    return w;
}

void JournalDialog::TryGetJourByUSB()
{
    QString filetofind;
    int jourtype = GetJourNum(sender()->objectName());
    BaseInterface::iface()->reqFile(DataTypes::FilesEnum(jourtype));
    //    switch (jourtype)
    //    {
    //    case DataTypes::JourSys:
    //        filetofind = "system.dat";
    //        break;
    //    case DataTypes::JourWork:
    //        filetofind = "workj.dat";
    //        break;
    //    case DataTypes::JourMeas:
    //        filetofind = "measj.dat";
    //        break;
    //    default:
    //        ERMSG("Incorrect jour type");
    //        return;
    //        break;
    //    }

    //    JourType = jourtype;
    //    JourFuncs->SetJourType(DataTypes::FilesEnum(jourtype));
    //    // QByteArray ba;
    //    QStringList drives = Files::Drives();
    //    if (!drives.isEmpty())
    //    {
    //        QStringList files = Files::SearchForFile(drives, filetofind);
    //        if (!files.isEmpty())
    //        {
    //            JourFile = Files::GetFirstDriveWithLabel(files, "AVM");
    //            JourFuncs->SetJourFile(JourFile);
    //            if (JourFile.isEmpty())
    //                GetJour();
    //            else
    //                StartReadJourFile();
    //        }
    //        else
    //            GetJour();
    //    }
    //    else
    //        GetJour();
}

void JournalDialog::GetJour()
{
    QByteArray ba;
    BaseInterface::iface()->reqFile(JourType);
    emit StartGetJour();
}

void JournalDialog::JourFileChoosed(QString &file)
{
    JourFile = file;
}

void JournalDialog::EraseJour()
{
    //    if (Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    if (WriteCheckPassword() == Error::Msg::NoError)
        //        {
        BaseInterface::iface()->writeCommand(Queries::QC_EraseJournals);
    //            int jourtype = GetJourNum(sender()->objectName());
    //            if (jourtype == INT_MAX)
    //            {
    //                ERMSG("Ошибочный тип журнала");
    //                return;
    //            }
    //            char num = jourtype;

    //            if (Commands::EraseTechBlock(num) == Error::Msg::NoError)
    //            {
    //                QMessageBox::information(this, "Успешно", "Стирание прошло успешно");
    //            }
    //            else
    //            {
    //                QMessageBox::information(this, "Ошибка", "Ошибка");
    //            }
    //        }
}

void JournalDialog::SaveJour()
{
    QString jourfilestr;
    QString tvname;
    int jtype = GetJourNum(sender()->objectName());
    switch (jtype)
    {
    case DataTypes::JourSys:
        tvname = "system";
        jourfilestr += "SysJ ";
        break;
    case DataTypes::JourWork:
        tvname = "work";
        jourfilestr += "WorkJ ";
        break;
    case DataTypes::JourMeas:
        tvname = "meas";
        jourfilestr += "MeasJ ";
        break;
    default:
        QMessageBox::critical(this, "Ошибка", "Ошибочный тип журнала");
        return;
    }

    QAbstractItemModel *amdl = WDFunc::TVModel(this, tvname);
    if (amdl == nullptr)
    {
        QMessageBox::critical(this, "Ошибка", "Данные ещё не получены");
        return;
    }
    // jourfilestr += QString::number(MTypeB, 16) + QString::number(MTypeM, 16) + " #"
    //    + QString("%1").arg(ModuleBSI::SerialNum(BoardTypes::BT_MODULE), 8, 10, QChar('0')) + " ";
    const auto &board = Board::GetInstance();
    jourfilestr += QString::number(board.typeB(), 16) + QString::number(board.typeM(), 16) + " #"
        + QString("%1").arg(board.serialNumber(Board::BaseAdd), 8, 10, QChar('0')) + " ";
    jourfilestr += QDate::currentDate().toString("dd-MM-yyyy") + ".xlsx";
    // запрашиваем имя файла для сохранения
    QString filename = Files::ChooseFileForSave(nullptr, "Excel documents (*.xlsx)", "xlsx", jourfilestr);

    progress->setMinimumDuration(0);
    connect(JourFuncs, &Journals::resendMaxResult, progress, &QProgressDialog::setMaximum);
    connect(JourFuncs, &Journals::resendResult, progress, &QProgressDialog::setValue);
    emit StartSaveJour(jtype, amdl, filename);
}

int JournalDialog::GetJourNum(const QString &objname)
{
    bool ok;
    QStringList sl = objname.split(".");

    if (sl.size() < 2)
    {
        qDebug() << "sl.size() < 2";
        return INT_MAX;
    }
    int jourtype = sl.at(1).toInt(&ok);
    QString jourprefix = sl.at(0);
    if (((jourprefix != "gj") && (jourprefix != "ej") && (jourprefix != "sj") && (jourprefix != "mj")) || !ok)
    {
        qDebug("Bad jourprefix");
        return INT_MAX;
    }
    return jourtype;
}

// void JournalDialog::setConnections()
//{
//}

bool JournalDialog::WriteCheckPassword()
{
    KeyPressDialog dlg; // = new KeyPressDialog;
    return dlg.CheckPassword("121941");
    //    ok = false;
    //    StdFunc::ClearCancel();
    //    QEventLoop PasswordLoop;
    //    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    //    connect(dlg, &KeyPressDialog::Finished, this, &JournalDialog::WritePasswordCheck);
    //    connect(this, &JournalDialog::WritePasswordChecked, &PasswordLoop, &QEventLoop::quit);
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
}

void JournalDialog::StartReadJourFile()
{
    progress->setMinimumDuration(0);
    connect(JourFuncs, &Journals::resendMaxResult, progress, &QProgressDialog::setMaximum);
    connect(JourFuncs, &Journals::resendResult, progress, &QProgressDialog::setValue);
    emit StartReadFile();
}

// void JournalDialog::WritePasswordCheck(QString psw)
//{
//    if (psw == "121941")
//        ok = true;
//    else
//        ok = false;
//    emit WritePasswordChecked();
//}

void JournalDialog::Done(QString msg, int)
{
    Q_UNUSED(msg)
    // new QAbstractItemModelTester(ProxyWorkModel, QAbstractItemModelTester::FailureReportingMode::Warning, this);

    disconnect(JourFuncs, &Journals::resendMaxResult, this->progress, &QProgressDialog::setMaximum);
    disconnect(JourFuncs, &Journals::resendResult, this->progress, &QProgressDialog::setValue);
    // QMessageBox::information(this, "Успешно", msg);
}

void JournalDialog::Error(QString msg)
{
    disconnect(JourFuncs, &Journals::resendMaxResult, this->progress, &QProgressDialog::setMaximum);
    disconnect(JourFuncs, &Journals::resendResult, this->progress, &QProgressDialog::setValue);
    qCritical() << msg;
    QMessageBox::critical(this, "Ошибка", msg);
}

// void JournalDialog::SetModel(ETableModel *mdl)
//{
//    // WW->SetMessage("Сортировка...");
//    MsgBox->setText("Сортировка...");
//    QSortFilterProxyModel *pmdl = new QSortFilterProxyModel;
//    pmdl->setSourceModel(mdl);
//    int dateidx = mdl->headerPosition("Дата/Время UTC");
//    switch (JourType)
//    {
//    case Files::JourWork:
//        WDFunc::SetTVModel(this, "work", pmdl, true);
//        WDFunc::SortTV(this, "work", dateidx, Qt::DescendingOrder);
//        break;
//    case Files::JourSys:
//        WDFunc::SetTVModel(this, "system", pmdl, true);
//        WDFunc::SortTV(this, "system", dateidx, Qt::DescendingOrder);
//        break;
//    case Files::JourMeas:
//        WDFunc::SetTVModel(this, "meas", pmdl, true);
//        WDFunc::SortTV(this, "meas", dateidx, Qt::AscendingOrder);
//        break;
//    }
//    // Done("Прочитано успешно");
//}
