#include <QDateTime>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QApplication>
#include <QSortFilterProxyModel>
#include <QFileDialog>

#include "../widgets/emessagebox.h"
#include "../gen/maindef.h"
#include "../models/getoscpbdelegate.h"
#include "../widgets/wd_func.h"
#include "../gen/stdfunc.h"
#include "../gen/error.h"
#include "../gen/timefunc.h"
#include "../dialogs/journalsdialog.h"
#include "../gen/colors.h"
#include "../gen/files.h"
#include "../gen/commands.h"

JournalDialog::JournalDialog(IEC104 *iec, QWidget *parent) : QDialog(parent)
{
    WW = new WaitWidget;
    JourFuncs = new Journals;
    connect(JourFuncs,SIGNAL(Done()),this,SLOT(Done()));
    connect(JourFuncs,SIGNAL(Error(QString)),this,SLOT(Error(QString)));
    connect(JourFuncs,SIGNAL(ResultReady(ETableModel *)),this,SLOT(Ready(ETableModel *)));
    connect(JourFuncs,SIGNAL(ReadJour(char)), iec, SLOT(SelectFile(char)));
    connect(iec,SIGNAL(SendJourSysfromiec104(QByteArray)), JourFuncs, SLOT(FillSysJour(QByteArray)));
    connect(iec,SIGNAL(SendJourWorkfromiec104(QByteArray)), JourFuncs, SLOT(FillWorkJour(QByteArray)));
    connect(iec,SIGNAL(SendJourMeasfromiec104(QByteArray)), JourFuncs, SLOT(FillMeasJour(QByteArray)));
    JourFuncs->moveToThread(&JourThread);
    JourThread.start();
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

JournalDialog::~JournalDialog()
{
    JourThread.quit();
    JourThread.wait();
}

void JournalDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
/*    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout; */
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";

/*    QTabWidget *work = new QTabWidget;
    work->tabBar()->setStyleSheet(ConfTWss);
    hlyout->addWidget(WDFunc::NewPB(this, "gj." + QString::number(Journals::JOURWORK), "Получить рабочий журнал", this, SLOT(TryGetJourByUSB())));
    hlyout->addWidget(WDFunc::NewPB(this, "ej." + QString::number(Journals::JOURWORK), "Стереть рабочий журнал", this, SLOT(EraseJour())));
    hlyout->addWidget(WDFunc::NewPB(this, "sj." + QString::number(Journals::JOURWORK), "Сохранить журнал в файл", this, SLOT(SaveJour())));
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::NewTV(this, "work", nullptr), 89);
    work->setLayout(vlyout);
    work->setStyleSheet(ConfTWss);

    QTabWidget *system = new QTabWidget;
    system->tabBar()->setStyleSheet(ConfTWss);

    hlyout = new QHBoxLayout;
    vlyout = new QVBoxLayout;
    hlyout->addWidget(WDFunc::NewPB(this, "gj." + QString::number(Journals::JOURSYS), "Получить системный журнал", this, SLOT(TryGetJourByUSB())));
    hlyout->addWidget(WDFunc::NewPB(this, "ej." + QString::number(Journals::JOURSYS), "Стереть системный журнал", this, SLOT(EraseJour())));
    hlyout->addWidget(WDFunc::NewPB(this, "sj." + QString::number(Journals::JOURSYS), "Сохранить журнал в файл", this, SLOT(SaveJour())));
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::NewTV(this, "system", nullptr), 89);
    system->setLayout(vlyout);
    system->setStyleSheet(ConfTWss);

    QTabWidget *measure = new QTabWidget;
    measure->tabBar()->setStyleSheet(ConfTWss);
    hlyout = new QHBoxLayout;
    vlyout = new QVBoxLayout;
    hlyout->addWidget(WDFunc::NewPB(this, "gj." + QString::number(Journals::JOURMEAS), "Получить журнал измерений", this, SLOT(TryGetJourByUSB())));
    hlyout->addWidget(WDFunc::NewPB(this, "ej." + QString::number(Journals::JOURMEAS), "Стереть журнал измерений", this, SLOT(EraseJour())));
    hlyout->addWidget(WDFunc::NewPB(this, "sj." + QString::number(Journals::JOURMEAS), "Сохранить журнал в файл", this, SLOT(SaveJour())));
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::NewTV(this, "meas", nullptr), 89);
    measure->setLayout(vlyout);
    measure->setStyleSheet(ConfTWss);
*/
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw4");
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(JourTab(Journals::JOURWORK), "Рабочий журнал");
    ConfTW->addTab(JourTab(Journals::JOURSYS),"Системный журнал");
    ConfTW->addTab(JourTab(Journals::JOURMEAS),"Журнал измерений");

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
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QWidget *w = new QWidget;
    QString str, tvname;

    switch(jourtype)
    {
    case Journals::JOURWORK:
        str = "рабочий журнал";
        tvname = "work";
        break;
    case Journals::JOURSYS:
        str = "системный журнал";
        tvname = "system";
        break;
    case Journals::JOURMEAS:
        str = "журнал измерений";
        tvname = "meas";
        break;
    }

    hlyout->addWidget(WDFunc::NewPB(this, "gj." + QString::number(jourtype), "Получить " + str, this, SLOT(TryGetJourByUSB())));
    hlyout->addWidget(WDFunc::NewPB(this, "ej." + QString::number(jourtype), "Стереть " + str, this, SLOT(EraseJour())));
    hlyout->addWidget(WDFunc::NewPB(this, "sj." + QString::number(jourtype), "Сохранить журнал в файл", this, SLOT(SaveJour())));
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::NewTV(this, tvname, nullptr), 89);
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
    QByteArray ba;
    QStringList drives = Files::Drives();
    if (!drives.isEmpty())
    {
        QStringList files = Files::SearchForFile(drives, filetofind);
        if (!files.isEmpty())
        {
            JourFile = Files::GetFirstDriveWithLabel(files, "AVM");
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
    char num = JourType + 4;
    if (MainInterface == I_ETHERNET)
    {
        emit ReadJour(num);
    }
    else if (MainInterface == I_USB)
    {
        QByteArray ba;
        // Крутилка
        WaitWidget *ww = new WaitWidget;
        ww->SetMessage("Чтение файла..");
        ww->Start();
        if(Commands::GetFile(num, ba) == NOERROR)
        {
            ww->Stop();
            switch(JourType)
            {
            case Journals::JOURSYS:
                FillEventsTable(ba, Journals::JOURSYS);
                break;
            case Journals::JOURWORK:
                FillEventsTable(ba, Journals::JOURWORK);
                break;
            case Journals::JOURMEAS:
                FillMeasTable(ba, Journals::JOURMEAS);
                break;
            default:
                break;
            }
        }
        else
        {
            ww->Stop();
            EMessageBox::information(this, "Ошибка", "Ошибка чтения журнала");
        }
    }
}

void JournalDialog::JourFileChoosed(QString &file)
{
    JourFile = file;
}

void JournalDialog::EraseJour()
{
    if (MainInterface == I_USB)
    {
        if(WriteCheckPassword() == NOERROR)
        {
            int jourtype = GetJourNum(sender()->objectName());
            if (jourtype == GENERALERROR)
            {
                ERMSG("Ошибочный тип журнала");
                return;
            }
            char num = jourtype + 4;

            if(Commands::EraseTechBlock(num) == NOERROR)
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
    QXlsx::Format cellformat;
    QString tvname, jourtypestr, jourfilestr;
    Qt::SortOrder order = Qt::AscendingOrder;
    int jourtype = GetJourNum(sender()->objectName());
    if (jourtype == GENERALERROR)
    {
        ERMSG("Ошибочный тип журнала");
        return;
    }
    jourfilestr = "KIV #" + QString("%1").arg(ModuleBSI::SerialNum(BoardTypes::BT_MODULE), 8, 10, QChar('0')) + " ";
    switch(jourtype)
    {
    case Journals::JOURSYS:
        tvname = "system";
        jourtypestr = "Системный журнал";
        jourfilestr += "SysJ ";
        order = Qt::DescendingOrder;
        cellformat.setNumberFormat("@");
        break;
    case Journals::JOURMEAS:
        tvname = "meas";
        jourtypestr = "Журнал измерений";
        jourfilestr += "MeasJ ";
        order = Qt::AscendingOrder;
        cellformat.setNumberFormat("#.####");
        break;
    case Journals::JOURWORK:
        tvname = "work";
        jourtypestr = "Журнал событий";
        jourfilestr += "WorkJ ";
        order = Qt::DescendingOrder;
        cellformat.setNumberFormat("@");
        break;
    default:
        break;
    }
    QAbstractItemModel *amdl = WDFunc::TVModel(this, tvname);
    if (amdl == nullptr)
    {
        EMessageBox::error(this, "Ошибка", "Данные ещё не получены");
        return;
    }
    QSortFilterProxyModel *pmdl = reinterpret_cast<QSortFilterProxyModel *>(amdl);
    ETableModel *mdl = reinterpret_cast<ETableModel *>(pmdl->sourceModel());
    int dateidx = mdl->Headers().indexOf("Дата/Время");
    pmdl->sort(dateidx, order);
    jourfilestr += QDate::currentDate().toString("dd-MM-yyyy") + ".xlsx";
    // запрашиваем имя файла для сохранения
    QString filename = Files::ChooseFileForSave(this, "Excel documents (*.xlsx)", "xlsx", jourfilestr);
    QXlsx::Document *xlsx = new QXlsx::Document(filename);
    xlsx->write(1,1,QVariant(jourtypestr));
    xlsx->write(2,1,QVariant("Модуль: " + ModuleBSI::GetModuleTypeString() + " сер. ном. " + \
                             QString::number(ModuleBSI::SerialNum(BoardTypes::BT_MODULE), 10)));
    xlsx->write(3,1,QVariant("Дата сохранения журнала: "+QDateTime::currentDateTime().toString("dd-MM-yyyy")));
    xlsx->write(4,1,QVariant("Время сохранения журнала: "+QDateTime::currentDateTime().toString("hh:mm:ss")));

    // пишем в файл заголовки
    for (int i=0; i<pmdl->columnCount(); ++i)
        xlsx->write(5, (i+1), pmdl->headerData(i, Qt::Horizontal, Qt::DisplayRole));

//    QXlsx::Format numformat; //, dateformat;
//    numformat.setNumberFormat("0");
//    dateformat.setNumberFormat("dd-mm-yyyy hh:mm:ss");
    // теперь по всем строкам модели пишем данные
    for (int i=0; i<pmdl->rowCount(); ++i)
    {
        // номер события
        xlsx->write((6+i), 1, pmdl->data(pmdl->index(i, 0), Qt::DisplayRole).toString());
        // время события
        xlsx->write((6+i), 2, pmdl->data(pmdl->index(i, 1), Qt::DisplayRole).toString());
        for (int j=2; j<pmdl->columnCount(); ++j)
            xlsx->write((6+i), (1+j), pmdl->data(pmdl->index(i, j), Qt::DisplayRole).toString(), cellformat);
//            xlsx->write((6+i), (1+j), pmdl->data(pmdl->index(i, j), Qt::DisplayRole).toString());
    }
    xlsx->save();
    EMessageBox::information(this, "Внимание", "Файл создан успешно");
}

void JournalDialog::FillSysJour(QByteArray ba)
{
    FillEventsTable(ba, Journals::JOURSYS);
}

void JournalDialog::FillMeasJour(QByteArray ba)
{
    quint32 crctocheck;
    quint32 basize = ba.size();
    if (basize < 17)
    {
        ERMSG("basize");
    }
    memcpy(&crctocheck, &(ba.data())[8], sizeof(quint32));
    if (!S2::CheckCRC32(&(ba.data())[16], (basize-16), crctocheck))
    {
        ERMSG("CRC error");
    }
    FillMeasTable(ba, Journals::JOURMEAS);
}

void JournalDialog::FillWorkJour(QByteArray ba)
{
    FillEventsTable(ba, Journals::JOURWORK);
}


void JournalDialog::FillEventsTable(QByteArray &ba, int jourtype)
{
}

void JournalDialog::FillMeasTable(QByteArray &ba, int jourtype)
{
    QVector<QVector<QVariant> > lsl;
    ETableModel *model = new ETableModel;
    QVector<QVariant> EventNum, Time, UeffA, UeffB, UeffC, IeffA, IeffB, IeffC, Freq, U0, U1, U2,
                      I0, I1, I2, CbushA, CbushB, CbushC, Tg_dA, Tg_dB, Tg_dC, dCbushA, dCbushB,
                      dCbushC, dTg_dA, dTg_dB, dTg_dC, Iunb, Phy_unb, Tmk, Tokr;

    MeasureStruct meas;
    int recordsize = sizeof(MeasureStruct);
    int basize = ba.size();
    char *file = ba.data();
    int joursize = 0; // размер считанного буфера с информацией
    int fhsize = sizeof(S2::FileHeader);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (jourtype == Journals::JOURMEAS)
    {
        file += fhsize;
        S2::DataRec jour;
        int drsize = sizeof(S2::DataRec) - sizeof(void *);
        memcpy(&jour, file, drsize);
        joursize = jour.num_byte;
        file += drsize; // move file pointer to thedata
    }
    int i = 0;
    while (i < basize)
    {
        memcpy(&meas, file, recordsize);
        file += recordsize;
        i += recordsize;


        if(meas.Time != 0xFFFFFFFF)
        {
            EventNum << meas.NUM;
            Time << TimeFunc::UnixTime32ToInvString(meas.Time);
            UeffA << meas.Ueff[0];
            UeffB << meas.Ueff[1];
            UeffC << meas.Ueff[2];
            IeffA << meas.Ieff[0];
            IeffB << meas.Ieff[1];
            IeffC << meas.Ieff[2];
            Freq << meas.Frequency;
            U0 << meas.U0;
            U1 << meas.U1;
            U2 << meas.U2;
            I0 << meas.I0;
            I1 << meas.I1;
            I2 << meas.I2;
            CbushA << meas.Cbush[0];
            CbushB << meas.Cbush[1];
            CbushC << meas.Cbush[2];
            Tg_dA << meas.Tg_d[0];
            Tg_dB << meas.Tg_d[1];
            Tg_dC << meas.Tg_d[2];
            dCbushA << meas.dCbush[0];
            dCbushB << meas.dCbush[1];
            dCbushC << meas.dCbush[2];
            dTg_dA << meas.dTg_d[0];
            dTg_dB << meas.dTg_d[1];
            dTg_dC << meas.dTg_d[2];
            Iunb << meas.Iunb;
            Phy_unb << meas.Phy_unb;
            Tmk << meas.Tmk;
            Tokr << meas.Tamb;
        }
    }
   lsl.append(EventNum);
   lsl.append(Time);
   lsl.append(UeffA);
   lsl.append(UeffB);
   lsl.append(UeffC);
   lsl.append(IeffA);
   lsl.append(IeffB);
   lsl.append(IeffC);
   lsl.append(Freq);
   lsl.append(U0);
   lsl.append(U1);
   lsl.append(U2);
   lsl.append(I0);
   lsl.append(I1);
   lsl.append(I2);
   lsl.append(CbushA);
   lsl.append(CbushB);
   lsl.append(CbushC);
   lsl.append(Tg_dA);
   lsl.append(Tg_dB);
   lsl.append(Tg_dC);
   lsl.append(dCbushA);
   lsl.append(dCbushB);
   lsl.append(dCbushC);
   lsl.append(dTg_dA);
   lsl.append(dTg_dB);
   lsl.append(dTg_dC);
   lsl.append(Iunb);
   lsl.append(Phy_unb);
   lsl.append(Tmk);
   lsl.append(Tokr);

   model->ClearModel();
   model->addColumn("Номер события");
   model->addColumn("Дата/Время");
/*   model->addColumn("Действующее значение напряжения фазы А, кВ");
   model->addColumn("Действующее значение напряжения фазы B, кВ");
   model->addColumn("Действующее значение напряжения фазы C, кВ");
   model->addColumn("Действующее значение тока фазы А, мА");
   model->addColumn("Действующее значение тока фазы B, мА");
   model->addColumn("Действующее значение тока фазы C, мА");
   model->addColumn("Напряжение нулевой последовательности");
   model->addColumn("Напряжение прямой последовательности");
   model->addColumn("Напряжение обратной последовательности");
   model->addColumn("Ток нулевой последовательности");
   model->addColumn("Ток прямой последовательности");
   model->addColumn("Ток обратной последовательности");
   model->addColumn("Емкость ввода фазы А, пФ");
   model->addColumn("Емкость ввода фазы B, пФ");
   model->addColumn("Емкость ввода фазы C, пФ");
   model->addColumn("tg delta ввода фазы А, %");
   model->addColumn("tg delta ввода фазы B, %");
   model->addColumn("tg delta ввода фазы C, %");
   model->addColumn("Изменение емкости ввода фазы А, % от C_init");
   model->addColumn("Изменение емкости ввода фазы B, % от C_init");
   model->addColumn("Изменение емкости ввода фазы C, % от C_init");
   model->addColumn("Изменение tg delta ввода фазы А, %");
   model->addColumn("Изменение tg delta ввода фазы B, %");
   model->addColumn("Изменение tg delta ввода фазы C, %");
   model->addColumn("Действующее значение 1-й гармоники тока небаланса, мА");
   model->addColumn("Угол тока небаланса относительно тока ф.А, град");
   model->addColumn("Температура кристалла микроконтроллера");
   model->addColumn("Температура окружающей среды"); */
   model->addColumn("Ueff фA");
   model->addColumn("Ueff фB");
   model->addColumn("Ueff фC");
   model->addColumn("Ieff фA");
   model->addColumn("Ieff фB");
   model->addColumn("Ieff фC");
   model->addColumn("Freq");
   model->addColumn("U0");
   model->addColumn("U1");
   model->addColumn("U2");
   model->addColumn("I0");
   model->addColumn("I1");
   model->addColumn("I2");
   model->addColumn("Cbush фA");
   model->addColumn("Cbush фB");
   model->addColumn("Cbush фC");
   model->addColumn("Tg_d фA");
   model->addColumn("Tg_d фB");
   model->addColumn("Tg_d фC");
   model->addColumn("dCbush фA");
   model->addColumn("dCbush фB");
   model->addColumn("dCbush фC");
   model->addColumn("dTg_d фA");
   model->addColumn("dTg_d фB");
   model->addColumn("dTg_d фC");
   model->addColumn("Iunb");
   model->addColumn("Phy_unb");
   model->addColumn("Tmk, °С");
   model->addColumn("Tamb, °С");
   for (int i=2; i<model->columnCount(); ++i)
       model->SetColumnFormat(i, 4); // set 4 diits precision for all cells starting 2
   model->fillModel(lsl);
   QSortFilterProxyModel *pmdl = new QSortFilterProxyModel;
   pmdl->setSourceModel(model);
   int dateidx = model->Headers().indexOf("Дата/Время");
//   pmdl->sort(dateidx, Qt::AscendingOrder);
   WDFunc::SetTVModel(this, "meas", pmdl, true);
   WDFunc::SortTV(this, "meas", dateidx, Qt::AscendingOrder);
   QApplication::restoreOverrideCursor();
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
    connect(dlg,SIGNAL(Finished(QString)),this,SLOT(WritePasswordCheck(QString)));
    connect(this,SIGNAL(WritePasswordChecked()),&PasswordLoop,SLOT(quit()));
    dlg->show();
    PasswordLoop.exec();
    if (StdFunc::IsCancelled())
        return GENERALERROR;
    if(!ok)
    {
        EMessageBox::error(this, "Неправильно", "Пароль введён неверно");
        return GENERALERROR;
    }
    return NOERROR;
}

void JournalDialog::StartReadJourFile()
{
    // Крутилка
    WW->SetMessage("Чтение файла..");
    WW->Start();

}

void JournalDialog::WritePasswordCheck(QString psw)
{
    if (psw == "121941")
        ok = true;
    else
        ok = false;
    emit WritePasswordChecked();
}

void JournalDialog::Done()
{

}

void JournalDialog::Error(QString msg)
{
    EMessageBox::error(this, "Ошибка", msg);
    ERMSG(msg);
    WW->Stop();
}

void JournalDialog::Ready(ETableModel *mdl)
{
    QSortFilterProxyModel *pmdl = new QSortFilterProxyModel;
    pmdl->setSourceModel(mdl);
    int dateidx = mdl->Headers().indexOf("Дата/Время");
    switch(JourType)
    {
    case Journals::JOURWORK:
        WDFunc::SetTVModel(this, "work", pmdl, true);
        WDFunc::SortTV(this, "work", dateidx, Qt::DescendingOrder);
    case Journals::JOURSYS:
        WDFunc::SetTVModel(this, "system", pmdl, true);
        WDFunc::SortTV(this, "system", dateidx, Qt::DescendingOrder);
    case Journals::JOURMEAS:
        WDFunc::SetTVModel(this, "meas", pmdl, true);
        WDFunc::SortTV(this, "meas", dateidx, Qt::AscendingOrder);
    }
    QApplication::restoreOverrideCursor();
    WW->Stop();
}
