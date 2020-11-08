#include "abstractcheckdialog.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "../widgets/etabwidget.h"
#include "../widgets/wd_func.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QTabWidget>
#include <QTime>
#include <QVBoxLayout>
#include <QtMath>
#include <QtTest/QTest>

AbstractCheckDialog::AbstractCheckDialog(QWidget *parent) : UDialog(parent)
{
    m_newTWIndex = 0;
    XlsxWriting = false;
    m_readDataInProgress = false;
    xlsx = nullptr;
    WRow = 0;
    m_oldTabIndex = m_currentTabIndex = 0;
    Timer = new QTimer(this);
    Timer->setObjectName("checktimer");
    connect(Timer, &QTimer::timeout, this, &AbstractCheckDialog::TimerTimeout);
    Timer->setInterval(1000);
    setAttribute(Qt::WA_DeleteOnClose);
}

AbstractCheckDialog::~AbstractCheckDialog()
{
    qDeleteAll(Bd_blocks);
    Bd_blocks.clear();
}

// void AbstractCheckDialog::SetupUI(QStringList &tabnames)
void AbstractCheckDialog::SetupUI()
{
    //    IndexWd.clear();
    //    if (tabnames.size() < BdUINum)
    //    {
    //        ERMSG("Wrong BdTab size");
    //        return;
    //    }
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *CheckTW = new QTabWidget;

    //    CheckTW->setObjectName("checktw" + QString::number(m_newTWIndex++));
    //    qDebug() << CheckTW->objectName();
    QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(Colors::TABCOLORA1) + ";}";

    //    QString ConfTWss = "QTabBar::tab {margin-right: 0px; margin-left: 0px; padding: 5px;}"
    //                       "QTabBar::tab:selected {background-color: "
    //        + QString(Colors::TABCOLORA1)
    //        + ";"
    //          "border: 1px solid #000000;"
    //          "border-top-left-radius: 4px;"
    //          "border-top-right-radius: 4px;"
    //          "padding: 2px;"
    //          "margin-left: -4px; margin-right: -4px;}"
    //          "QTabBar::tab:first:selected {margin-left: 0;}"
    //          "QTabBar::tab:last:selected {margin-right: 0;}"
    //          "QTabBar::tab:only-one {margin: 0;}";

    CheckTW->tabBar()->setStyleSheet(ConfTWss);
    //    CheckTW->addTab(AutoCheckUI(),"  Автоматическая проверка  ");
    //    for (int i = 0; i < BdUINum; ++i)
    //    {
    //        CheckTW->addTab(BdUI(i), "  " + tabnames.at(i) + "  ");
    //        IndexWd.append(i);
    //    }
    foreach (BdUIStruct w, m_BdUIList)
    {
        w.widget->setInterface(iface());
        CheckTW->addTab(w.widget, " " + w.widgetCaption + " ");
        connect(&DataManager::GetInstance(), &DataManager::floatReceived, w.widget, &UWidget::updateFloatData);
        connect(&DataManager::GetInstance(), &DataManager::singlePointReceived, w.widget, &UWidget::updateSPData);
    }
    //    QWidget *w = CustomTab();
    //    if (w != nullptr)
    //        CheckTW->addTab(w, "  Прочее  ");
    lyout->addWidget(CheckTW);
    // lyout->addWidget(BottomUI());
    setLayout(lyout);
    connect(CheckTW, &QTabWidget::tabBarClicked, this, &AbstractCheckDialog::TWTabClicked);
}

void AbstractCheckDialog::PrepareHeadersForFile(int row)
{
    Q_UNUSED(row)
}

void AbstractCheckDialog::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row)
    Q_UNUSED(bdnum)
}

void AbstractCheckDialog::PrepareAnalogMeasurements()
{
}

// QWidget *AbstractCheckDialog::CustomTab() { return nullptr; }

// void AbstractCheckDialog::Check1PPS()
//{
//}

void AbstractCheckDialog::SetBd(int bdnum, void *block, int blocksize, bool toxlsx)
{
    BdBlocks *bdblock = new BdBlocks;
    bdblock->block = block;
    bdblock->blocknum = blocksize;
    bdblock->toxlsxwrite = toxlsx;
    Bd_blocks[bdnum] = bdblock;
}

QWidget *AbstractCheckDialog::BottomUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel("Период обновления данных измерения, сек:");
    hlyout->addWidget(lbl);
    QRadioButton *rb = new QRadioButton;
    rb->setObjectName("1000");
    rb->setText("1");
    connect(rb, SIGNAL(clicked()), this, SLOT(SetTimerPeriod()));
    hlyout->addWidget(rb);
    rb = new QRadioButton;
    rb->setObjectName("2000");
    rb->setText("2");
    rb->setChecked(true);
    connect(rb, SIGNAL(clicked()), this, SLOT(SetTimerPeriod()));
    hlyout->addWidget(rb);
    rb = new QRadioButton;
    rb->setObjectName("10000");
    rb->setText("10");
    connect(rb, SIGNAL(clicked()), this, SLOT(SetTimerPeriod()));
    hlyout->addWidget(rb);
    lyout->addLayout(hlyout);

    QPushButton *pb = new QPushButton("Запустить чтение аналоговых сигналов");
    pb->setObjectName("pbmeasurements");
    connect(pb, SIGNAL(clicked()), this, SLOT(StartAnalogMeasurements()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    lyout->addWidget(pb);
    pb = new QPushButton("Запустить чтение аналоговых сигналов в файл");
    pb->setObjectName("pbfilemeasurements");
    connect(pb, SIGNAL(clicked()), this, SLOT(StartAnalogMeasurementsToFile()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    lyout->addWidget(pb);
    pb = new QPushButton("Остановить чтение аналоговых сигналов");
    connect(pb, SIGNAL(clicked()), this, SLOT(StopAnalogMeasurements()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    lyout->addWidget(pb);
    w->setLayout(lyout);
    return w;
}

void AbstractCheckDialog::StartAnalogMeasurementsToFile()
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString Filename = dlg->getSaveFileName(this, "Сохранить данные", StdFunc::GetHomeDir(), "Excel files (*.xlsx)",
        Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    dlg->close();
    if (Filename == "")
    {
        ERMSG("Не задано имя файла");
        return; // !!! ошибка - не задано имя файла
    }
    if (Filename.split(".").last() != "xlsx")
        Filename += ".xlsx";
    // удаляем файл, если он есть
    QFile fn;
    fn.setFileName(Filename);
    if (fn.exists())
        fn.remove();
    XlsxWriting = true;
    xlsx = new QXlsx::Document(Filename);
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "Модуль" : "Прибор");
    xlsx->write(1, 1,
        QVariant(tmps + ": " + ModuleBSI::GetModuleTypeString() + " сер. ном. "
            + QString::number(ModuleBSI::SerialNum(BoardTypes::BT_MODULE), 10)));
    xlsx->write(2, 1, QVariant("Дата начала записи: " + QDateTime::currentDateTime().toString("dd-MM-yyyy")));
    xlsx->write(3, 1, QVariant("Время начала записи: " + QDateTime::currentDateTime().toString("hh:mm:ss")));
    xlsx->write(5, 1, QVariant("Дата и время отсчёта"));
    PrepareHeadersForFile(6); // в 6 ряду пишем заголовки
    WRow = 7;
    QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
    if (pb != nullptr)
        pb->setEnabled(false);
    pb = this->findChild<QPushButton *>("pbmeasurements");
    if (pb != nullptr)
        pb->setEnabled(false);
    ElapsedTimeCounter = new QElapsedTimer;
    ElapsedTimeCounter->start();
    StartAnalogMeasurements();
}

void AbstractCheckDialog::ReadAnalogMeasurementsAndWriteToFile()
{

    // получение текущих аналоговых сигналов от модуля
    if (m_readDataInProgress)
    {
        ERMSG("Ещё не завершена предыдущая операция");
        return;
    }

    m_readDataInProgress = true;
    if (XlsxWriting)
    {
        xlsx->write(WRow, 1, QVariant(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")));
        QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
        if (pb != nullptr)
        {
            int MSecs = ElapsedTimeCounter->elapsed();
            QString TimeElapsed = QTime::fromMSecsSinceStartOfDay(MSecs).toString("hh:mm:ss.zzz");
            pb->setText("Идёт запись: " + TimeElapsed);
        }
    }
    int bdkeyssize = Bd_blocks.keys().size();
    for (int bdnum = 0; bdnum < bdkeyssize; ++bdnum)
    {
        if (XlsxWriting && (Bd_blocks[Bd_blocks.keys().at(bdnum)]->toxlsxwrite))
        {
            if (XlsxWriting)
                WriteToFile(WRow, bdnum);
        }
    }

    WRow++;
    m_readDataInProgress = false;
}

// void AbstractCheckDialog::StartBdMeasurements() { BdTimer->start(); }

// void AbstractCheckDialog::StopBdMeasurements() { BdTimer->stop(); }

// void AbstractCheckDialog::onModbusStateChanged()
//{
//    if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
//        QMessageBox::information(this, "Успешно", "Связь по MODBUS установлена");
//}

void AbstractCheckDialog::StartAnalogMeasurements()
{
    //    CurBdNum = 1;
    PrepareAnalogMeasurements();
    Timer->start();
}

void AbstractCheckDialog::StopAnalogMeasurements()
{
    if (XlsxWriting)
    {
        if (xlsx)
        {
            xlsx->save();
            QMessageBox::information(this, "Внимание", "Файл создан успешно");
            delete xlsx;
        }
        QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
        if (pb != nullptr)
        {
            pb->setEnabled(true);
            pb->setText("Запустить чтение аналоговых сигналов в файл");
        }
        pb = this->findChild<QPushButton *>("pbmeasurements");
        if (pb != nullptr)
            pb->setEnabled(true);
        XlsxWriting = false;
    }
    Timer->stop();
}

void AbstractCheckDialog::reqUpdate()
{
    foreach (BdUIStruct bd, m_BdUIList)
    {
        bd.widget->reqUpdate();
    }
}

void AbstractCheckDialog::TimerTimeout()
{
    ReadAnalogMeasurementsAndWriteToFile();
}

void AbstractCheckDialog::TWTabClicked(int index)
{
    if (index == m_currentTabIndex) // to prevent double function invocation by doubleclicking on tab
        return;
    m_currentTabIndex = index;

    if (m_oldTabIndex >= m_BdUIList.size())
    {
        DBGMSG("BdUIList size");
        return;
    }
    UWidget *w = m_BdUIList.at(m_oldTabIndex).widget;
    w->setUpdatesDisabled();
    if (m_currentTabIndex >= m_BdUIList.size())
    {
        DBGMSG("BdUIList size");
        return;
    }
    w = m_BdUIList.at(m_currentTabIndex).widget;
    w->setUpdatesEnabled();
    w->reqUpdate();
    m_oldTabIndex = m_currentTabIndex;
}

void AbstractCheckDialog::SetTimerPeriod()
{
    bool TimerIsActive = false;
    if (Timer->isActive())
        TimerIsActive = true;
    bool ok;
    int per = sender()->objectName().toInt(&ok);
    if (!ok)
    {
        ERMSG("Ошибка считывания интервала таймера");
        return;
    }
    Timer->stop();
    Timer->setInterval(per);
    if (TimerIsActive)
        Timer->start();
}
