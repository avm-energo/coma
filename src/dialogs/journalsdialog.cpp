#include "../dialogs/journalsdialog.h"

#include "../dialogs/keypressdialog.h"
#include "../module/board.h"
#include "../widgets/QProgressIndicator.h"
#include "../widgets/wd_func.h"

#ifdef MODELDEBUG
#include <QAbstractItemModelTester>
#endif

#include <QDateTime>
#include <QDebug>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <gen/colors.h>
#include <gen/datamanager/datamanager.h>
#include <gen/datamanager/typesproxy.h>
#include <gen/error.h>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>

namespace crypto
{
static constexpr char hash[] = "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc";
static constexpr char name[] = "jourHash";
}

JournalDialog::JournalDialog(UniquePointer<Journals> jour, QWidget *parent)
    : UDialog(crypto::hash, crypto::name, parent), m_jour(std::move(jour))
{
    auto mngr = &DataManager::GetInstance();
    static DataTypesProxy proxy(mngr);
    proxy.RegisterType<DataTypes::FileStruct>();
    connect(&proxy, &DataTypesProxy::DataStorable, m_jour.get(), &Journals::FillJour);
    proxyWorkModel = new QSortFilterProxyModel(this);
    proxySysModel = new QSortFilterProxyModel(this);
    proxyMeasModel = new QSortFilterProxyModel(this);
    progress = new QProgressDialog(this);
    progress->setCancelButton(nullptr);
    progress->cancel();

    m_jour->SetProxyModels(proxyWorkModel, proxySysModel, proxyMeasModel);

    connect(m_jour.get(), &Journals::Done, this, &JournalDialog::done);
    connect(m_jour.get(), &Journals::Error, this, &JournalDialog::error);
    connect(this, &JournalDialog::startSaveJour, m_jour.get(), &Journals::saveJour);

    setupUI();
}

JournalDialog::~JournalDialog()
{
}

void JournalDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw4");
    ConfTW->addTab(jourTab(DataTypes::JourWork), "Рабочий журнал");
    ConfTW->addTab(jourTab(DataTypes::JourSys), "Системный журнал");
    ConfTW->addTab(jourTab(DataTypes::JourMeas), "Журнал измерений");

    lyout->addWidget(ConfTW);
    setLayout(lyout);
}

QWidget *JournalDialog::jourTab(DataTypes::FilesEnum jourtype)
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
        mdl = proxyWorkModel;
        break;
    case DataTypes::JourSys:
        str = "системный журнал";
        tvname = "system";
        mdl = proxySysModel;
        break;
    case DataTypes::JourMeas:
        str = "журнал измерений";
        tvname = "meas";
        mdl = proxyMeasModel;
        break;
    default:
        qCritical() << "Unknown journal type: " << jourtype;
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
        this, "sj." + QString::number(jourtype), "Сохранить журнал в файл", this, [=] { saveJour(jourtype); });
    saveButton->setEnabled(false);
    connect(m_jour.get(), &Journals::Done, this, [saveButton](const QString &str, const int &number) {
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

void JournalDialog::jourFileChoosed(QString &file)
{
    JourFile = file;
}

void JournalDialog::eraseJour()
{
    if (checkPassword())
        BaseInterface::iface()->writeCommand(Queries::QC_EraseJournals);
}

void JournalDialog::saveJour(DataTypes::FilesEnum jourType)
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
    emit startSaveJour(jourType, filename);
}

int JournalDialog::getJourNum(const QString &objname)
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

void JournalDialog::done(QString msg)
{
    disconnect(m_jour.get(), &Journals::resendMaxResult, progress, &QProgressDialog::setMaximum);
    disconnect(m_jour.get(), &Journals::resendResult, progress, &QProgressDialog::setValue);
    setSuccessMsg(msg);
    // QMessageBox::information(this, "Успешно", msg);
}

void JournalDialog::error(QString msg)
{
    disconnect(m_jour.get(), &Journals::resendMaxResult, progress, &QProgressDialog::setMaximum);
    disconnect(m_jour.get(), &Journals::resendResult, progress, &QProgressDialog::setValue);
    qCritical() << msg;
    setErrorMsg(msg);
    QMessageBox::critical(this, "Ошибка", msg);
}
