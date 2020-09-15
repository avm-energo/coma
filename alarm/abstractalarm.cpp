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

AbstractAlarm::AbstractAlarm(QDialog *parent) : QDialog(parent)
{
    //    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
}

void AbstractAlarm::UpdatePixmaps(quint32 alarm, int counter)
{
    if (alarm)
    {
        auto pixmap = WDFunc::NewCircle(Qt::red, CIRCLE_RADIUS);
        WDFunc::SetLBLImage(this, (QString::number(counter)), &pixmap);
    }
    else
    {
        auto pixmap = WDFunc::NewCircle(Qt::green, CIRCLE_RADIUS);
        WDFunc::SetLBLImage(this, (QString::number(counter)), &pixmap);
    }
}

void AbstractAlarm::showEvent(QShowEvent *e)
{
    if (isVisible())
        move(QCursor::pos());
    e->accept();
}

AbstractWarnAlarm::AbstractWarnAlarm(QDialog *parent) : AbstractAlarm(parent)
{
}

void AbstractWarnAlarm::Update(QList<bool> states)
{
    // qDebug() << Board::GetInstance()->type();
    if (states.isEmpty())
        return;
    auto max_range = std::min(Alarm->MapAlarm.value(Board::GetInstance()->type()).warnCounts, states.length());
    for (int i = 0; i < max_range; i++)
    {
        UpdatePixmaps(states.at(i), i);
    }
}
max_range AbstractAvarAlarm::AbstractAvarAlarm(QDialog *parent) : AbstractAlarm(parent)
{
}

void AbstractAvarAlarm::Update(QList<bool> states)
{
    // qDebug() << Board::GetInstance()->type();
    if (states.isEmpty())
        return;
    auto max_range = std::min(Alarm->MapAlarm.value(Board::GetInstance()->type()).avarCounts, states.length());
    for (int i = 0; i < max_range; i++)
    {
        UpdatePixmaps(states.at(i), i);
    }
}
