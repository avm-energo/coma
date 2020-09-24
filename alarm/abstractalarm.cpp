#include "abstractalarm.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../widgets/wd_func.h"

#include <QApplication>
#include <QBoxLayout>
#include <QCursor>
#include <QDialog>
#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QStringListModel>
#include <QToolBar>

AbstractAlarm::AbstractAlarm(QWidget *parent) : QDialog(parent)
{
    //    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
}

void AbstractAlarm::UpdatePixmaps(quint32 alarm, int counter, bool warn)
{

    if (alarm)
    {
        if (warn)
        {
            auto pixmap = WDFunc::NewCircle(Qt::yellow, CIRCLE_RADIUS);
            WDFunc::SetLBLImage(this, (QString::number(counter)), &pixmap);
        }
        else
        {
            auto pixmap = WDFunc::NewCircle(Qt::red, CIRCLE_RADIUS);
            WDFunc::SetLBLImage(this, (QString::number(counter)), &pixmap);
        }
    }
    else
    {
        auto pixmap = WDFunc::NewCircle(Qt::green, CIRCLE_RADIUS);
        WDFunc::SetLBLImage(this, (QString::number(counter)), &pixmap);
    }
}

void AbstractAlarm::SetupAlarm(const QStringList &events, int counters)
{
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");

    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    auto max_range = std::min(events.size(), counters);
    for (int i = 0; i < max_range; ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "transparent", QString::number(i)));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    w->setLayout(vlayout);

    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb, &QAbstractButton::clicked, this, &QWidget::hide);
    lyout->addWidget(pb, 0);
    setLayout(lyout);
}

void AbstractAlarm::showEvent(QShowEvent *e)
{
    if (isVisible())
        move(QCursor::pos());
    e->accept();
}

AbstractWarnAlarm::AbstractWarnAlarm(QWidget *parent) : AbstractAlarm(parent)
{
}

void AbstractWarnAlarm::Update(QList<bool> states)
{
    if (states.isEmpty())
        return;
    auto max_range = states.length();
    Q_ASSERT(Alarm->MapAlarm.value(Board::GetInstance()->type()).warnCounts == states.length());
    max_range = std::min(Alarm->MapAlarm.value(Board::GetInstance()->type()).warnCounts, states.length());
    // qDebug() << Alarm->MapAlarm.value(Board::GetInstance()->type()).warnCounts << ":" << states.length();
    for (int i = 0; i < max_range; i++)
    {
        UpdatePixmaps(states.at(i), i, true);
    }
}
AbstractAvarAlarm::AbstractAvarAlarm(QWidget *parent) : AbstractAlarm(parent)
{
}

void AbstractAvarAlarm::Update(QList<bool> states)
{
    if (states.isEmpty())
        return;
    auto max_range = states.length();
    if (Alarm->MapAlarm.value(Board::GetInstance()->type()).avarCounts != states.length())
    {
        qDebug() << Alarm->MapAlarm.value(Board::GetInstance()->type()).avarCounts << ":" << states.length();
        max_range = std::min(Alarm->MapAlarm.value(Board::GetInstance()->type()).avarCounts, states.length());
    }
    for (int i = 0; i < max_range; i++)
    {
        UpdatePixmaps(states.at(i), i);
    }
}
