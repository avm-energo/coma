#include "../dialogs/journalsdialog.h"

//#include "../dialogs/keypressdialog.h"
#include "../journals/measjournal.h"
#include "../journals/sysjournal.h"
#include "../journals/workjournal.h"
#include "../module/board.h"
#include "../s2/s2datatypes.h"
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

JournalDialog::JournalDialog(const ModuleSettings &settings, QWidget *parent)
    : UDialog(crypto::hash, crypto::name, parent), proxy(&DataManager::GetInstance())
{
    disableSuccessMessage();
    proxy.RegisterType<S2DataTypes::S2BFile>();
    createJournals(settings);

    progress = new QProgressDialog(this);
    progress->setCancelButton(nullptr);
    progress->cancel();
    setupUI();
}

void JournalDialog::createJournals(const ModuleSettings &settings)
{
    using namespace journals;
    journals.reserve(4);
    journals.append(new SysJournal(this));
    if (!settings.getWorkJours().isEmpty())
        journals.append(new WorkJournal(settings.getWorkJours(), this));
    if (!settings.getMeasJours().isEmpty())
        journals.append(new MeasJournal(settings.getMeasJours(), this));

    for (auto &journal : journals)
    {
        connect(journal, &Journal::done, this, &JournalDialog::done);
        connect(journal, &Journal::error, this, &JournalDialog::error);
    }
}

void JournalDialog::tabChanged(const int index)
{
    disconnect(&proxy, &DataTypesProxy::DataStorable, journals[currentTab], &Journal::fill);
    disconnect(this, &JournalDialog::startSaveJour, journals[currentTab], &Journal::save);
    currentTab = index;
    connect(&proxy, &DataTypesProxy::DataStorable, journals[currentTab], &Journal::fill);
    connect(this, &JournalDialog::startSaveJour, journals[currentTab], &Journal::save);
}

void JournalDialog::setupUI()
{
    auto layout = new QVBoxLayout;
    auto tabWidget = new QTabWidget;
    // tabWidget->setObjectName("conftw4");

    for (auto &journal : journals)
    {
        auto journalTab = jourTab(journal);
        tabWidget->addTab(journalTab, journal->getName());
    }
    layout->addWidget(tabWidget);
    setLayout(layout);

    connect(tabWidget, &QTabWidget::currentChanged, this, &JournalDialog::tabChanged);
    currentTab = 0;
    connect(&proxy, &DataTypesProxy::DataStorable, journals[currentTab], &Journal::fill);
    connect(this, &JournalDialog::startSaveJour, journals[currentTab], &Journal::save);
}

QWidget *JournalDialog::jourTab(const Journal *journal)
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QWidget *w = new QWidget(this);
    auto str = journal->getName().toLower();
    auto modelView = journal->createModelView(this);
    auto jourtype = journal->getType();

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
        BaseInterface::iface()->reqFile(quint32(jourtype));
        modelView->setUpdatesEnabled(false);
    });
    connect(journal, &Journal::done, progressDialog, &QWidget::close);
    connect(journal, &Journal::done, modelView, [modelView] { modelView->setUpdatesEnabled(true); });
    hlyout->addWidget(getButton);
    QPushButton *eraseButton = WDFunc::NewPB(this, "ej." + QString::number(jourtype), "Стереть " + str, this, [=] {
        if (checkPassword())
            BaseInterface::iface()->writeCommand(Commands::C_EraseJournals, quint8(jourtype));
    });
    hlyout->addWidget(eraseButton);
    QPushButton *saveButton = WDFunc::NewPB(this, "sj." + QString::number(jourtype), "Сохранить журнал в файл", this,
        [=] { saveJour(DataTypes::FilesEnum(jourtype)); });
    saveButton->setEnabled(false);
    connect(journal, &Journal::done, this, [saveButton](const QString &str) {
        Q_UNUSED(str)
        saveButton->setEnabled(true);
    });
    hlyout->addWidget(saveButton);
    vlyout->addLayout(hlyout);
    vlyout->addWidget(modelView, 89);
    w->setLayout(vlyout);
    return w;
}

// void JournalDialog::jourFileChoosed(QString &file)
//{
//    JourFile = file;
//}

void JournalDialog::eraseJour()
{
    if (checkPassword())
        BaseInterface::iface()->writeCommand(Commands::C_EraseJournals);
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
    connect(journals[currentTab], &Journal::resendMaxResult, progress, &QProgressDialog::setMaximum);
    connect(journals[currentTab], &Journal::resendResult, progress, &QProgressDialog::setValue);
    emit startSaveJour(filename);
}

// int JournalDialog::getJourNum(const QString &objname)
//{
//    bool ok = false;
//    QStringList sl = objname.split(".");
//    if (sl.size() < 2)
//    {
//        qDebug() << "sl.size() < 2";
//        return INT_MAX;
//    }
//    int jourtype = sl.at(1).toInt(&ok);
//    const QString jourprefix = sl.at(0);
//    if (((jourprefix != "gj") && (jourprefix != "ej") && (jourprefix != "sj") && (jourprefix != "mj")) || !ok)
//    {
//        qDebug("Bad jourprefix");
//        return INT_MAX;
//    }
//    return jourtype;
//}

void JournalDialog::done(const QString &msg)
{
    disconnect(journals[currentTab], &Journal::resendMaxResult, progress, &QProgressDialog::setMaximum);
    disconnect(journals[currentTab], &Journal::resendResult, progress, &QProgressDialog::setValue);
    setSuccessMsg(msg);
}

void JournalDialog::error(const QString &msg)
{
    disconnect(journals[currentTab], &Journal::resendMaxResult, progress, &QProgressDialog::setMaximum);
    disconnect(journals[currentTab], &Journal::resendResult, progress, &QProgressDialog::setValue);
    qCritical() << msg;
    setErrorMsg(msg);
    QMessageBox::critical(this, "Ошибка", msg);
}
