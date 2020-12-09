#include "waitwidget.h"

#include "../gen/stdfunc.h"

#include <QApplication>
#include <QBitmap>
#include <QDateTime>
#include <QDesktopWidget>
#include <QFontMetrics>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <QVBoxLayout>
#include <math.h>

WaitWidget::WaitWidget(QWidget *parent) : QWidget(parent)
{
    //    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("QWidget {background: rgb(0, 186, 144);}");
    setAttribute(Qt::WA_DeleteOnClose);
    gamma = 0.0;
    Message = "";
    resize(310, 380);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    srand(QDateTime::currentDateTime().toSecsSinceEpoch());
    vel1 = 0.5;
    IsAllowedToStop = false;
    IsIncrement = true;
    TimeFormat = WW_SIMPLE;
    Seconds = 0;
}

WaitWidget::~WaitWidget()
{
}

void WaitWidget::Init(WaitWidget::ww_struct &ww)
{
    this->IsAllowedToStop = ww.isallowedtostop;
    this->IsIncrement = ww.isincrement;
    this->TimeFormat = ww.format;
    this->Seconds = ww.initialseconds;
    QFont font;
    QFontMetrics fm(font);
    PressAnyKeyStringWidth = fm.horizontalAdvance(PressAnyKeyString); //+10;
}

void WaitWidget::Start()
{
    show();
    QTimer *tmr = new QTimer;
    tmr->setInterval(10);
    connect(tmr, SIGNAL(timeout()), this, SLOT(Rotate()));
    tmr->start();
    QTimer *tmrs = new QTimer;
    tmrs->setInterval(1000);
    connect(tmrs, SIGNAL(timeout()), this, SLOT(UpdateSeconds()));
    tmrs->start();
}

void WaitWidget::Stop()
{
    emit finished();
    close();
}

void WaitWidget::SetMessage(QString msg)
{
    Message = msg;
}

void WaitWidget::SetSeconds(quint32 seconds)
{
    Seconds = seconds;
}

void WaitWidget::Rotate()
{
    gamma += 0.5;
    update();
}

void WaitWidget::UpdateSeconds()
{
    if (IsIncrement)
        ++Seconds;
    else
    {
        --Seconds;
        if (Seconds == 0)
        {
            //            emit CountZero();
            Stop();
        }
    }
}

void WaitWidget::paintEvent(QPaintEvent *e)
{
    QPixmap rotatedPixmap("images/Koma.png");
    QSize size = rotatedPixmap.size();
    QSize wsize = QSize(310, 380);
    int left = wsize.width() / 2 - size.width() / 2;
    QPainter p1(this);
    p1.setRenderHint(QPainter::Antialiasing);
    p1.translate(wsize.width() / 2, size.height() / 2);
    p1.rotate(gamma * vel1);
    p1.translate(-wsize.width() / 2, -size.height() / 2);
    p1.drawPixmap(left, 0, rotatedPixmap);
    p1.end();
    QPainter p(this);
    //    p.translate(wsize.width()/2,size.height()/2);
    QFont font;
    QFontMetrics fm(font);
    int center = wsize.width() / 2;
    int msgwidth = fm.horizontalAdvance(Message); //+10;
    left = center - msgwidth / 2;
    QRect mrect = QRect(left, height() - 20, msgwidth, 20);
    //    p.setPen(Qt::blue);
    //    QBrush brush(Qt::lightGray, Qt::SolidPattern);
    //    p.drawRect(mrect);
    //    p.fillRect(mrect, brush);
    p.setPen(Qt::black);
    p.drawText(mrect, Qt::AlignCenter, Message);
    if (IsAllowedToStop)
    {
        left = center - PressAnyKeyStringWidth / 2;
        mrect = QRect(left, height() - 40, PressAnyKeyStringWidth, 20);
        p.drawText(mrect, Qt::AlignCenter, PressAnyKeyString);
    }
    p.end();
    QPainter ps(this);
    QPen pen(Qt::darkGreen, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    ps.setPen(pen);
    font.setFamily("Helvetica");
    font.setPointSize(20);
    ps.setFont(font);
    QRect srect = QRect(0, height() - 65, width(), 24);
    QString SecondsString = (TimeFormat == WW_TIME)
        ? (QString::number(Seconds / 60) + ":" + QString::number(Seconds % 60))
        : QString::number(Seconds);
    ps.drawText(srect, Qt::AlignCenter, SecondsString);
    ps.end();
    e->accept();
}

void WaitWidget::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Escape) && IsAllowedToStop)
    {
        StdFunc::cancel();
        Stop();
        //        emit CountZero();
    }
    QWidget::keyPressEvent(e);
}

void WaitWidget::closeEvent(QCloseEvent *e)
{
    emit CountZero();
    e->accept();
}
