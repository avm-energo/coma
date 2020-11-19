#include "alarmwidget.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"

#include <QBoxLayout>
#include <QDebug>
#include <QGroupBox>
#include <QMenu>
#include <QtCore>
AlarmWidget::AlarmWidget(QWidget *parent) : UWidget(parent)
{
    //    Alarm = alarm;

    QMenu *menu = new QMenu;
    QString tmps = "QMenuBar {background-color: " + QString(Colors::MAINWINCLR)
        + ";}"
          "QMenuBar::item {background-color: "
        + QString(Colors::MAINWINCLR) + ";}";
    menu->setStyleSheet(tmps);
    QVBoxLayout *vlyout = new QVBoxLayout;

    QHBoxLayout *hlyout2 = new QHBoxLayout;
    QStringList Description { "Состояние устройства", "Предупредительная сигнализация", "Аварийная сигнализация" };
    QStringList ButtonList { "AlarmButtonPressed", "ModuleWarnButtonPressed", "ModuleAlarmButtonPressed" };
    setStyleSheet("QComa {background-color: " + QString(Colors::MAINWINCLR) + ";}");
    for (int i = 0; i != Description.size(); i++)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        QPushButton *pb = new QPushButton(Description.at(i));
        pb->setMinimumSize(QSize(geometry().width() / 3, 30));
        int signal = metaObject()->indexOfSignal(QString((ButtonList.at(i) + "()")).toStdString().c_str());
        auto method = metaObject()->method(signal);
        connect(pb, QMetaMethod::fromSignal(&QAbstractButton::clicked), this, method);
        QGroupBox *gb = new QGroupBox("");
        hlyout->addWidget(pb, Qt::AlignRight);
        QPixmap map;
        hlyout->addWidget(WDFunc::NewLBL(this, "", "", QString::number(950 + i), &map), 1);
        WDFunc::SetVisible(this, QString::number(950 + i), false);
        gb->setLayout(hlyout);
        hlyout2->addWidget(gb);
    }

    //    QHBoxLayout *hlyout = new QHBoxLayout;
    //    QPixmap map;
    //    hlyout->addWidget(WDFunc::NewLBL(this, "", "", QString::number(953), &map), 1);
    //    hlyout2->addLayout(hlyout);

    //    QHBoxLayout *hlyout = new QHBoxLayout;
    //    QPixmap map;
    //    hlyout->addWidget(WDFunc::NewLBL(this, "", "", QString::number(953), &map, "Индикатор включения машины"), 1);
    //    hlyout2->addLayout(hlyout);

    if (hlyout2->count())
        vlyout->addLayout(hlyout2);
    setLayout(vlyout);

    //    connect(Alarm, &AlarmClass::SetFirstButton, this, &AlarmWidget::UpdateFirstUSB);
    //    connect(Alarm, &AlarmClass::setWarnColor, this, &AlarmWidget::UpdateSecondUSB);
    //    connect(Alarm, &AlarmClass::setAlarmColor, this, &AlarmWidget::UpdateThirdUSB);
    //    connect(Alarm, &AlarmClass::SetIndicator, this, &AlarmWidget::UpdateIndicator);
}

void AlarmWidget::UpdateIndicator(bool indx)
{
    auto pixmap
        = WDFunc::NewLedIndicator((indx == 0) ? QColor(Qt::green) : QColor(0xE0, 0xE0, 0xE0), this->height() / 2);
    qDebug() << this->height();
    WDFunc::SetLBLImage(this, "953", &pixmap);
    WDFunc::SetVisible(this, "953", true);
}

// void AlarmWidget::update(bool w, bool a)
//{
//    UpdateFirstUSB();
//    UpdateSecondUSB(w);
//    UpdateThirdUSB(a);
//}

void AlarmWidget::updateWarn(bool isset)
{
    auto pixmap = WDFunc::NewCircle((isset) ? Qt::yellow : Qt::green, this->height() / 4);
    WDFunc::SetLBLImage(this, "951", &pixmap);
    WDFunc::SetVisible(this, "951", true);
}

void AlarmWidget::updateAlarm(bool isset)
{
    auto pixmap = WDFunc::NewCircle((isset) ? Qt::red : Qt::green, this->height() / 4);
    WDFunc::SetLBLImage(this, "952", &pixmap);
    WDFunc::SetVisible(this, "952", true);
}

void AlarmWidget::updateMain(bool isset)
{
    Qt::GlobalColor color;
    const auto &board = Board::GetInstance();
    if (board.health() & BSIALARMMASK)
        color = Qt::red;
    else if (board.health() & BSIWARNMASK)
        color = Qt::yellow;
    else
        color = Qt::green;
    auto pixmap = WDFunc::NewCircle(color, this->height() / 4);
    WDFunc::SetLBLImage(this, "950", &pixmap);
    WDFunc::SetVisible(this, "950", true);
}

// void AlarmWidget::UpdateSecondUSB(QList<bool> warnalarm)
// void AlarmWidget::UpdateSecondUSB(bool warn)
//{
//    //    // int i;
//    //    int alarm = 0;

//    //    // for(i=0; i<warnalarmcount.size(); i++)
//    //    foreach (bool item, warnalarm)
//    //    {
//    //        if (item == true)
//    //        {
//    //            alarm++;
//    //            break;
//    //        }
//    //    }
//    auto pixmap = WDFunc::NewCircle((warn) ? Qt::yellow : Qt::green, this->height() / 4);
//    WDFunc::SetLBLImage(this, "951", &pixmap);
//    WDFunc::SetVisible(this, "951", true);
//}

// void AlarmWidget::UpdateThirdUSB(QList<bool> avar)
// void AlarmWidget::UpdateThirdUSB(bool alarm)
//{
//    //    int alarm = 0;

//    //    //     for(i=0; i<alarmcount.size(); i++)
//    //    //    if(std::any_of(avar.constBegin(),avar.constEnd(),[](const bool state){return state;}))
//    //    //        alarm++;
//    //    foreach (bool item, avar)
//    //    {
//    //        if (item == true)
//    //        {
//    //            alarm++;
//    //            break;
//    //        }
//    //    }
//    auto pixmap = WDFunc::NewCircle((alarm) ? Qt::red : Qt::green, this->height() / 4);
//    WDFunc::SetLBLImage(this, "952", &pixmap);
//    WDFunc::SetVisible(this, "952", true);
//}

// void AlarmWidget::UpdateFirstUSB()
//{
//    Qt::GlobalColor color;
//    if (ModuleBSI::ModuleBsi.Hth & BSIALARMMASK)
//        color = Qt::red;
//    //    {
//    //        auto pixmap = WDFunc::NewCircle(Qt::red, this->height() / 4);
//    //        WDFunc::SetLBLImage(this, "950", &pixmap);
//    //        WDFunc::SetVisible(this, "950", true);
//    //    }
//    else if (ModuleBSI::ModuleBsi.Hth & BSIWARNMASK)
//        color = Qt::yellow;
//    //    {
//    //        auto pixmap = WDFunc::NewCircle(Qt::yellow, this->height() / 4);
//    //        WDFunc::SetLBLImage(this, "950", &pixmap);
//    //        WDFunc::SetVisible(this, "950", true);
//    //    }
//    else
//        color = Qt::green;
//    //    {
//    auto pixmap = WDFunc::NewCircle(color, this->height() / 4);
//    WDFunc::SetLBLImage(this, "950", &pixmap);
//    WDFunc::SetVisible(this, "950", true);
//    //    }
//}

void AlarmWidget::Clear()
{
    WDFunc::SetVisible(this, "950", false);
    WDFunc::SetVisible(this, "951", false);
    WDFunc::SetVisible(this, "952", false);
    WDFunc::SetVisible(this, "953", false);
}