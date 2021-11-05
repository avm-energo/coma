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
#include "../widgets/QProgressIndicator.h"

#include <QDateTime>
#include <QDebug>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
namespace crypto
{
static constexpr char hash[] = "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc";
static constexpr char name[] = "jourHash";
}
JournalDialog::JournalDialog(UniquePointer<Journals> jour, QWidget *parent)
    : UDialog(crypto::hash, crypto::name, parent), m_jour(std::move(jour))
{
    connect(&DataManager::GetInstance(), &DataManager::fileReceived, m_jour.get(), &Journals::FillJour);
    ProxyWorkModel = new QSortFilterProxyModel(this);
    ProxySysModel = new QSortFilterProxyModel(this);
    ProxyMeasModel = new QSortFilterProxyModel(this);
    progress = new QProgressDialog(this);
    progress->setCancelButton(nullptr);
    progress->cancel();

    m_jour->SetProxyModels(ProxyWorkModel, ProxySysModel, ProxyMeasModel);

    connect(m_jour.get(), &Journals::Done, this, &JournalDialog::Done);
    connect(m_jour.get(), &Journals::Error, this, &JournalDialog::Error);
    connect(this, &JournalDialog::StartSaveJour, m_jour.get(), &Journals::saveJour);

    SetupUI();
}

JournalDialog::~JournalDialog()
{
}

void JournalDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw4");
    ConfTW->addTab(JourTab(DataTypes::JourWork), "Рабочий журнал");
    ConfTW->addTab(JourTab(DataTypes::JourSys), "Системный журнал");
    ConfTW->addTab(JourTab(DataTypes::JourMeas), "Журнал измерений");

    lyout->addWidget(ConfTW);
    setLayout(lyout);
}

QWidget *JournalDialog::JourTab(DataTypes::FilesEnum jourtype)
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
    }
    auto *modelView = WDFunc::NewTV(this, tvname, mdl);
    QDialog *progressDialog = new QDialog(
        this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    QProgressIndicator *progressIndicator = new QProgressIndicator(this);
    hlyout->addWidget(progressIndicator);
    progressDialog->setLayout(hlyout);
    hlyout = new QHBoxLayout;
    progressIndicator->startAnimation();
    QPushButton *getButton = WDFunc::NewPB(this, "gj." + QString::number(jourtype), "Получить " + str, this, [=] {
        progressDialog->show();
        progressIndicator->startAnimation();
        BaseInterface::iface()->reqFile(DataTypes::FilesEnum(jourtype));
        modelView->setUpdatesEnabled(false);
    });
    connect(m_jour.get(), &Journals::Done, progressDialog, &QWidget::close);
    connect(m_jour.get(), &Journals::Done, modelView, [modelView] { modelView->setUpdatesEnabled(true); });
    hlyout->addWidget(getButton);
    QPushButton *eraseButton = WDFunc::NewPB(this, "ej." + QString::number(jourtype), "Стереть " + str, this, [=] {
        if (checkPassword())
            BaseInterface::iface()->writeCommand(Queries::QC_EraseJournals, quint8(jourtype));
    });
    hlyout->addWidget(eraseButton);
    QPushButton *saveButton = WDFunc::NewPB(
        this, "sj." + QString::number(jourtype), "Сохранить журнал в файл", this, [=] { SaveJour(jourtype); });
    saveButton->setEnabled(false);
    connect(m_jour.get(), &Journals::Done, [saveButton](const QString &str, const int &number) {
        Q_UNUSED(str)
        if (saveButton->objectName().back().digitValue() == number)
            saveButton->setEnabled(true);
    });
    hlyout->addWidget(saveButton);
    vlyout->addLayout(hlyout);

    views.insert(DataTypes::FilesEnum(jourtype), modelView);
    vlyout->addWidget(modelView, 89);
    w->setLayout(vlyout);
    return w;
}

void JournalDialog::JourFileChoosed(QString &file)
{
    JourFile = file;
}

void JournalDialog::EraseJour()
{
    if (checkPassword())
        BaseInterface::iface()->writeCommand(Queries::QC_EraseJournals);
}

void JournalDialog::SaveJour(DataTypes::FilesEnum jourType)
{
    QString jourfilestr;
    QString tvname;
    switch (jourType)
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
    QString filename = WDFunc::ChooseFileForSave(nullptr, "Excel documents (*.xlsx)", "xlsx", jourfilestr);

    progress->setMinimumDuration(0);
    connect(m_jour.get(), &Journals::resendMaxResult, progress, &QProgressDialog::setMaximum);
    connect(m_jour.get(), &Journals::resendResult, progress, &QProgressDialog::setValue);
    emit StartSaveJour(jourType, filename);
}

int JournalDialog::GetJourNum(const QString &objname)
{
    bool ok = false;
    QStringList sl = objname.split(".");

    if (sl.size() < 2)
    {
        qDebug() << "sl.size() < 2";
        return INT_MAX;
    }
    int jourtype = sl.at(1).toInt(&ok);
    const QString jourprefix = sl.at(0);
    if (((jourprefix != "gj") && (jourprefix != "ej") && (jourprefix != "sj") && (jourprefix != "mj")) || !ok)
    {
        qDebug("Bad jourprefix");
        return INT_MAX;
    }
    return jourtype;
}

// void JournalDialog::StartReadJourFile()
//{
//    progress->setMinimumDuration(0);
//    connect(m_jour.get(), &Journals::resendMaxResult, progress, &QProgressDialog::setMaximum);
//    connect(m_jour.get(), &Journals::resendResult, progress, &QProgressDialog::setValue);
//    emit StartReadFile();
//}

void JournalDialog::Done(QString msg)
{

    disconnect(m_jour.get(), &Journals::resendMaxResult, this->progress, &QProgressDialog::setMaximum);
    disconnect(m_jour.get(), &Journals::resendResult, this->progress, &QProgressDialog::setValue);
    QMessageBox::information(this, "Успешно", msg);
}

void JournalDialog::Error(QString msg)
{
    disconnect(m_jour.get(), &Journals::resendMaxResult, this->progress, &QProgressDialog::setMaximum);
    disconnect(m_jour.get(), &Journals::resendResult, this->progress, &QProgressDialog::setValue);
    qCritical() << msg;
    QMessageBox::critical(this, "Ошибка", msg);
}
