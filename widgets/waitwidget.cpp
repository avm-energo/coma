#include <QLabel>
#include <QTimer>
#include <QPixmap>
#include <QBitmap>
#include <QImage>
#include <QPainter>
#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDateTime>
#include <QFontMetrics>
#include "waitwidget.h"
#include <math.h>

WaitWidget::WaitWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    gamma = 0.0;
    Message = "";
    resize(800, 220);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    qsrand(QDateTime::currentDateTime().toTime_t());
    vel1 = 0.5;
    vel2 = -1;
    vel3 = 1;
    vel4 = static_cast<float>(-0.2);
    vel5 = 3.5;
}

WaitWidget::~WaitWidget()
{

}

void WaitWidget::Start()
{
    show();
    QTimer *tmr = new QTimer;
    tmr->setInterval(10);
    connect(tmr,SIGNAL(timeout()),this,SLOT(Rotate()));
    tmr->start();
}

void WaitWidget::Stop()
{
    emit finished();
    this->close();
}

void WaitWidget::SetMessage(QString msg)
{
    Message = msg;
}

void WaitWidget::Rotate()
{
    gamma += 0.5;
/*    vel1 += (static_cast<float>(qrand()%100)-50)/5000; // числа от -0.01 до 0.01
    vel2 += (static_cast<float>(qrand()%100)-50)/5000; // числа от -0.01 до 0.01
    vel3 += (static_cast<float>(qrand()%100)-50)/5000; // числа от -0.01 до 0.01
    vel4 += (static_cast<float>(qrand()%100)-50)/5000; // числа от -0.01 до 0.01
    vel5 += (static_cast<float>(qrand()%100)-50)/5000; // числа от -0.01 до 0.01
    Message = QString::number(vel1); */
    update();
}

void WaitWidget::paintEvent(QPaintEvent *e)
{
    QPixmap rotatedPixmap(":/pic/Object1-0.png");
    QSize size = rotatedPixmap.size();
    QSize wsize = QSize(800,220);
    int left = wsize.width()/2-size.width()/2;
    QPainter p1(this);
    p1.setRenderHint(QPainter::Antialiasing);
    p1.translate(wsize.width()/2,size.height()/2);
    p1.rotate(gamma*vel1);
    p1.translate(-wsize.width()/2,-size.height()/2);
    p1.drawPixmap(left, 0, rotatedPixmap);
    p1.end();
    QPixmap rotatedPixmap2(":/pic/Object2.png");
    QPainter p2(this);
    p2.setRenderHint(QPainter::Antialiasing);
    p2.translate(wsize.width()/2,size.height()/2);
    p2.rotate(gamma*vel2);
    p2.translate(-wsize.width()/2,-size.height()/2);
    p2.drawPixmap(left, 0, rotatedPixmap2);
    p2.end();
    QPixmap rotatedPixmap3(":/pic/Object3.png");
    QPainter p3(this);
    p3.setRenderHint(QPainter::Antialiasing);
    p3.translate(wsize.width()/2,size.height()/2);
    p3.rotate(gamma*vel3);
    p3.translate(-wsize.width()/2,-size.height()/2);
    p3.drawPixmap(left, 0, rotatedPixmap3);
    p3.end();
    QPixmap rotatedPixmap4(":/pic/Object4.png");
    QPainter p4(this);
    p4.setRenderHint(QPainter::Antialiasing);
    p4.translate(wsize.width()/2,size.height()/2);
    p4.rotate(gamma*vel4);
    p4.translate(-wsize.width()/2,-size.height()/2);
    p4.drawPixmap(left, 0, rotatedPixmap4);
    p4.end();
    QPixmap rotatedPixmap5(":/pic/Object5.png");
    QPainter p5(this);
    p5.setRenderHint(QPainter::Antialiasing);
    p5.translate(wsize.width()/2,size.height()/2);
    p5.rotate(gamma*vel5);
    p5.translate(-wsize.width()/2,-size.height()/2);
    p5.drawPixmap(left, 0, rotatedPixmap5);
    p5.end();
    QPainter p(this);
//    p.translate(wsize.width()/2,size.height()/2);
    QFont font;
    QFontMetrics fm(font);
    int center = wsize.width()/2;
    int msgwidth = fm.width(Message);//+10;
    left = center - msgwidth/2;
    QRect mrect = QRect(left,height()-20,msgwidth,20);
    p.setPen(Qt::blue);
    QBrush brush(Qt::lightGray, Qt::SolidPattern);
    p.drawRect(mrect);
    p.fillRect(mrect, brush);
    p.setPen(Qt::black);
    p.drawText(mrect, Qt::AlignCenter, Message);
    p.end();
    e->accept();
}
