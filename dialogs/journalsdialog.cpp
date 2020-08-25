#include "../dialogs/journalsdialog.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/maindef.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../models/getoscpbdelegate.h"
#include "../usb/commands.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"

#include <QAbstractItemModelTester>
#include <QApplication>
#include <QDateTime>
#include <QFileDialog>
#include <QGroupBox>
#include <QHeaderView>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#if _MSC_VER && !__INTEL_COMPILER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
JournalDialog::JournalDialog(IEC104 *iec, QWidget *parent) : QDialog(parent)
{
    JourFuncs = new Journals;
    ProxyWorkModel = new QSortFilterProxyModel;
    ProxySysModel = new QSortFilterProxyModel;
    ProxyMeasModel = new QSortFilterProxyModel;
    JourFuncs->SetProxyModels(ProxyWorkModel, ProxySysModel, ProxyMeasModel);
    //    JourFuncs->SetParentWidget(this);
    // connect(JourFuncs, &Journals::Done, this, &JournalDialog::Done, Qt::BlockingQueuedConnection);
    connect(JourFuncs, &Journals::Done, this, &JournalDialog::Done);
    connect(JourFuncs, &Journals::Error, this, &JournalDialog::Error);
    // connect(JourFuncs, &Journals::ModelReady, this, &JournalDialog::SetModel);
    //    connect(JourFuncs,SIGNAL(ProxyModelReady(QSortFilterProxyModel
    //    *)),this,SLOT(SetProxyModel(QSortFilterProxyModel *)));
    connect(JourFuncs, &Journals::ReadJour, iec, &IEC104::SelectFile);
    connect(iec, &IEC104::SendJourSysfromiec104, JourFuncs, &Journals::FillSysJour);
    connect(iec, &IEC104::SendJourWorkfromiec104, JourFuncs, &Journals::FillWorkJour);
    connect(iec, &IEC104::SendJourMeasfromiec104, JourFuncs, &Journals::FillMeasJour);
    connect(this, &JournalDialog::StartGetJour, JourFuncs, &Journals::StartGetJour);
    connect(this, &JournalDialog::StartSaveJour, JourFuncs, &Journals::StartSaveJour);
    connect(this, &JournalDialog::StartReadFile, JourFuncs, &Journals::ReadJourFileAndProcessIt);
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
    QString tmps = "QDialog {background-color: " + QString(ACONFCLR) + ";}";
    setStyleSheet(tmps);
    QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(TABCOLOR) + ";}";

    /*    QTabWidget *work = new QTabWidget;
        work->tabBar()->setStyleSheet(ConfTWss);
        hlyout->addWidget(WDFunc::NewPB(this, "gj." +
       QString::number(Journals::JOURWORK), "Получить рабочий журнал", this,
       SLOT(TryGetJourByUSB()))); hlyout->addWidget(WDFunc::NewPB(this, "ej." +
       QString::number(Journals::JOURWORK), "Стереть рабочий журнал", this,
       SLOT(EraseJour()))); hlyout->addWidget(WDFunc::NewPB(this, "sj." +
       QString::number(Journals::JOURWORK), "Сохранить журнал в файл", this,
       SLOT(SaveJour()))); vlyout->addLayout(hlyout);
        vlyout->addWidget(WDFunc::NewTV(this, "work", nullptr), 89);
        work->setLayout(vlyout);
        work->setStyleSheet(ConfTWss);

        QTabWidget *system = new QTabWidget;
        system->tabBar()->setStyleSheet(ConfTWss);

        hlyout = new QHBoxLayout;
        vlyout = new QVBoxLayout;
        hlyout->addWidget(WDFunc::NewPB(this, "gj." +
       QString::number(Journals::JOURSYS), "Получить системный журнал", this,
       SLOT(TryGetJourByUSB()))); hlyout->addWidget(WDFunc::NewPB(this, "ej." +
       QString::number(Journals::JOURSYS), "Стереть системный журнал", this,
       SLOT(EraseJour()))); hlyout->addWidget(WDFunc::NewPB(this, "sj." +
       QString::number(Journals::JOURSYS), "Сохранить журнал в файл", this,
       SLOT(SaveJour()))); vlyout->addLayout(hlyout);
        vlyout->addWidget(WDFunc::NewTV(this, "system", nullptr), 89);
        system->setLayout(vlyout);
        system->setStyleSheet(ConfTWss);

        QTabWidget *measure = new QTabWidget;
        measure->tabBar()->setStyleSheet(ConfTWss);
        hlyout = new QHBoxLayout;
        vlyout = new QVBoxLayout;
        hlyout->addWidget(WDFunc::NewPB(this, "gj." +
       QString::number(Journals::JOURMEAS), "Получить журнал измерений", this,
       SLOT(TryGetJourByUSB()))); hlyout->addWidget(WDFunc::NewPB(this, "ej." +
       QString::number(Journals::JOURMEAS), "Стереть журнал измерений", this,
       SLOT(EraseJour()))); hlyout->addWidget(WDFunc::NewPB(this, "sj." +
       QString::number(Journals::JOURMEAS), "Сохранить журнал в файл", this,
       SLOT(SaveJour()))); vlyout->addLayout(hlyout);
        vlyout->addWidget(WDFunc::NewTV(this, "meas", nullptr), 89);
        measure->setLayout(vlyout);
        measure->setStyleSheet(ConfTWss);
    */
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw4");
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(JourTab(Journals::JOURWORK), "Рабочий журнал");
    ConfTW->addTab(JourTab(Journals::JOURSYS), "Системный журнал");
    ConfTW->addTab(JourTab(Journals::JOURMEAS), "Журнал измерений");

    lyout->addWidget(ConfTW);
    setLayout(lyout);
    if (StdFunc::IsInEmulateMode())
    {
        WDFunc::SetEnabled(this, "gj." + QString::number(Journals::JOURSYS), false);
        WDFunc::SetEnabled(this, "gj." + QString::number(Journals::JOURMEAS), false);
        WDFunc::SetEnabled(this, "gj." + QString::number(Journals::JOURWORK), false);
        WDFunc::SetEnabled(this, "ej." + QString::number(Journals::JOURSYS), false);
        WDFunc::SetEnabled(this, "ej." + QString::number(Journals::JOURMEAS), false);
        WDFunc::SetEnabled(this, "ej." + QString::number(Journals::JOURWORK), false);
        WDFunc::SetEnabled(this, "sj." + QString::number(Journals::JOURSYS), false);
        WDFunc::SetEnabled(this, "sj." + QString::number(Journals::JOURMEAS), false);
        WDFunc::SetEnabled(this, "sj." + QString::number(Journals::JOURWORK), false);
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
    case Journals::JOURWORK:
        str = "рабочий журнал";
        tvname = "work";
        mdl = ProxyWorkModel;
        break;
    case Journals::JOURSYS:
        str = "системный журнал";
        tvname = "system";
        mdl = ProxySysModel;
        break;
    case Journals::JOURMEAS:
        str = "журнал измерений";
        tvname = "meas";
        mdl = ProxyMeasModel;
        break;
    default:
        DBGMSG;
        return w;
        break;
    }

    hlyout->addWidget(
        WDFunc::NewPB(this, "gj." + QString::number(jourtype), "Получить " + str, this, SLOT(TryGetJourByUSB())));
    hlyout->addWidget(
        WDFunc::NewPB(this, "ej." + QString::number(jourtype), "Стереть " + str, this, SLOT(EraseJour())));
    hlyout->addWidget(
        WDFunc::NewPB(this, "sj." + QString::number(jourtype), "Сохранить журнал в файл", this, SLOT(SaveJour())));
    vlyout->addLayout(hlyout);
    auto *modelView = WDFunc::NewTV(this, tvname, mdl);
    // modelView->setSortingEnabled(true);
    vlyout->addWidget(modelView, 89);
    w->setLayout(vlyout);
    return w;
}

void JournalDialog::TryGetJourByUSB()
{
    QString filetofind;
    int jourtype = GetJourNum(sender()->objectName());
    if (jourtype == GENERALERROR)
    {
        ERMSG("Incorrect jour type");
        return;
    }
    if (jourtype == Journals::JOURSYS)
        filetofind = "system.dat";
    else if (jourtype == Journals::JOURWORK)
        filetofind = "workj.dat";
    else if (jourtype == Journals::JOURMEAS)
        filetofind = "measj.dat";
    else
    {
        ERMSG("Incorrect jour type");
        return;
    }
    JourType = jourtype;
    JourFuncs->SetJourType(jourtype);
    // QByteArray ba;
    QStringList drives = Files::Drives();
    if (!drives.isEmpty())
    {
        QStringList files = Files::SearchForFile(drives, filetofind);
        if (!files.isEmpty())
        {
            JourFile = Files::GetFirstDriveWithLabel(files, "AVM");
            JourFuncs->SetJourFile(JourFile);
            if (JourFile.isEmpty())
                GetJour();
            else
                StartReadJourFile();
        }
        else
            GetJour();
    }
    else
        GetJour();
}

void JournalDialog::GetJour()
{
    emit StartGetJour();
}

void JournalDialog::JourFileChoosed(QString &file)
{
    JourFile = file;
}

void JournalDialog::EraseJour()
{
    if (MainInterface == I_USB)
    {
        if (WriteCheckPassword() == NOERROR)
        {
            int jourtype = GetJourNum(sender()->objectName());
            if (jourtype == GENERALERROR)
            {
                ERMSG("Ошибочный тип журнала");
                return;
            }
            char num = jourtype + 4;

            if (Commands::EraseTechBlock(num) == NOERROR)
            {
                EMessageBox::information(this, "Успешно", "Стирание прошло успешно");
            }
            else
            {
                EMessageBox::information(this, "Ошибка", "Ошибка");
            }
        }
    }
}

void JournalDialog::SaveJour()
{
    QString jourfilestr;
    QString tvname;
    int jtype = GetJourNum(sender()->objectName());
    if (jtype == GENERALERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибочный тип журнала");
        return;
    }

    if (jtype == Journals::JOURSYS)
    {
        tvname = "system";
        jourfilestr += "SysJ ";
    }
    else if (jtype == Journals::JOURWORK)
    {
        tvname = "work";
        jourfilestr += "WorkJ ";
    }
    else
    {
        tvname = "meas";
        jourfilestr += "MeasJ ";
    }

    QAbstractItemModel *amdl = WDFunc::TVModel(this, tvname);
    if (amdl == nullptr)
    {
        EMessageBox::error(this, "Ошибка", "Данные ещё не получены");
        return;
    }
    jourfilestr += QString::number(MTypeB, 16) + QString::number(MTypeM, 16) + " #"
        + QString("%1").arg(ModuleBSI::SerialNum(BoardTypes::BT_MODULE), 8, 10, QChar('0')) + " ";
    jourfilestr += QDate::currentDate().toString("dd-MM-yyyy") + ".xlsx";
    // запрашиваем имя файла для сохранения
    QString filename = Files::ChooseFileForSave(nullptr, "Excel documents (*.xlsx)", "xlsx", jourfilestr);
    // WW = new WaitWidget;
    // WW->SetMessage("Запись файла...");
    // WW->Start();
    emit StartSaveJour(jtype, amdl, filename);
}

int JournalDialog::GetJourNum(const QString &objname)
{
    bool ok;
    QStringList sl = objname.split(".");

    if (sl.size() < 2)
    {
        DBGMSG;
        return GENERALERROR;
    }
    int jourtype = sl.at(1).toInt(&ok);
    QString jourprefix = sl.at(0);
    if (((jourprefix != "gj") && (jourprefix != "ej") && (jourprefix != "sj") && (jourprefix != "mj")) || !ok)
    {
        DBGMSG;
        return GENERALERROR;
    }
    return jourtype;
}

int JournalDialog::WriteCheckPassword()
{
    ok = false;
    StdFunc::ClearCancel();
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    connect(dlg, SIGNAL(Finished(QString)), this, SLOT(WritePasswordCheck(QString)));
    connect(this, SIGNAL(WritePasswordChecked()), &PasswordLoop, SLOT(quit()));
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

void JournalDialog::StartReadJourFile()
{
    //    QApplication::setOverrideCursor(Qt::WaitCursor);
    // Крутилка
    // WW = new WaitWidget(nullptr);
    // MsgBox = new QMessageBox(QMessageBox::Warning, "Внимание", "Идет чтение журнала...");
    progress = new QProgressDialog;
    progress->setMinimumDuration(0);
    connect(JourFuncs, &Journals::resendMaxResult, this->progress, &QProgressDialog::setMaximum);
    connect(JourFuncs, &Journals::resendResult, this->progress, &QProgressDialog::setValue);
    progress->setLabelText("Loading model...");
    // MsgBox->setText("");
    emit StartReadFile();
    // MsgBox->exec();
    // WW->SetMessage("Чтение файла..");
    // WW->Start();
}

void JournalDialog::WritePasswordCheck(QString psw)
{
    if (psw == "121941")
        ok = true;
    else
        ok = false;
    emit WritePasswordChecked();
}

void JournalDialog::Done(QString msg)
{
    qDebug() << __PRETTY_FUNCTION__;
    // new QAbstractItemModelTester(ProxyWorkModel, QAbstractItemModelTester::FailureReportingMode::Warning, this);

    // if (WW != nullptr && MainInterface == I_USB)
    //    WW->Stop();
    if (progress != nullptr && MainInterface == I_USB)
    {
        // progress->reset();
        // MsgBox->close();
        qDebug() << "Close";
    }
    // update();
    disconnect(JourFuncs, &Journals::resendMaxResult, this->progress, &QProgressDialog::setMaximum);
    disconnect(JourFuncs, &Journals::resendResult, this->progress, &QProgressDialog::setValue);
    // MsgBox->closeEvent(QMessageBox::DestructiveRole);
    //    QApplication::restoreOverrideCursor();
    // EMessageBox::information(this, "Успешно", msg);
}

void JournalDialog::Error(QString msg)
{
    //    if (WW != nullptr && MainInterface == I_USB)
    //        WW->Stop();
    if (progress != nullptr && MainInterface == I_USB)
    {
        qDebug() << "Error";
    }
    disconnect(JourFuncs, &Journals::resendMaxResult, this->progress, &QProgressDialog::setMaximum);
    disconnect(JourFuncs, &Journals::resendResult, this->progress, &QProgressDialog::setValue);
    ERMSG(msg);
    //    QApplication::restoreOverrideCursor();
    EMessageBox::error(this, "Ошибка", msg);
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
//    case Journals::JOURWORK:
//        WDFunc::SetTVModel(this, "work", pmdl, true);
//        WDFunc::SortTV(this, "work", dateidx, Qt::DescendingOrder);
//        break;
//    case Journals::JOURSYS:
//        WDFunc::SetTVModel(this, "system", pmdl, true);
//        WDFunc::SortTV(this, "system", dateidx, Qt::DescendingOrder);
//        break;
//    case Journals::JOURMEAS:
//        WDFunc::SetTVModel(this, "meas", pmdl, true);
//        WDFunc::SortTV(this, "meas", dateidx, Qt::AscendingOrder);
//        break;
//    }
//    // Done("Прочитано успешно");
//}
