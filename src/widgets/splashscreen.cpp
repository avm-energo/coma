#include "splashscreen.h"

#include <QCoreApplication>
#include <QIcon>
#include <QPainter>
#include <QString>
SplashScreen::SplashScreen() : QSplashScreen {}
{
}

SplashScreen::SplashScreen(const QPixmap &pixmap, Qt::WindowFlags f) : QSplashScreen(pixmap, f)
{
}

void SplashScreen::drawContents(QPainter *painter)
{
    const QIcon jourIcon(":/icons/coma.svg");
    const int margin = 20;
    QPixmap pix = jourIcon.pixmap(QSize(200, 200));

    painter->drawPixmap(QRect(margin, margin, pix.width(), pix.height()), pix);

    QFont font = painter->font();
    font.setPixelSize(16);
    painter->setFont(font);

    QRect rect(margin, margin, painter->window().width() - 2 * margin, painter->font().pixelSize() * 1.2);
    QString name;
    name.append(Prog::name_ru)
        .append(" (")
        .append(QCoreApplication::applicationName())
        .append(") ")
        .append(QCoreApplication::applicationVersion());

    painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, name);
    rect.moveBottom(rect.bottom() + 2 * painter->font().pixelSize());
    painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, Prog::department);

    font.setPixelSize(10);
    painter->setFont(font);
    rect.setHeight(painter->font().pixelSize() * 1.2);

    rect.moveBottom(rect.bottom() + 1.7 * painter->font().pixelSize());
    painter->drawText(rect, Qt::AlignLeft, Prog::modules);

    rect.moveBottom(rect.bottom() + 1.7 * painter->font().pixelSize());
    painter->drawText(rect, Qt::AlignLeft, Prog::zlib);

    rect.moveBottom(rect.bottom() + 1.7 * painter->font().pixelSize());
    painter->drawText(rect, Qt::AlignLeft, Prog::hidapi);

    rect.moveBottom(rect.bottom() + 1.7 * painter->font().pixelSize());
    painter->drawText(rect, Qt::AlignLeft, Prog::qxlsx);

    rect.moveBottom(rect.bottom() + 1.7 * painter->font().pixelSize());
    painter->drawText(rect, Qt::AlignLeft, Prog::limereport);

    rect.moveBottom(rect.bottom() + 1.7 * painter->font().pixelSize());
    painter->drawText(rect, Qt::AlignLeft, Prog::icons);

    rect.moveBottom(rect.bottom() + 1.7 * painter->font().pixelSize());
    painter->drawText(rect, Qt::AlignLeft, Prog::fonts);

    font.setPixelSize(20);
    painter->setFont(font);

    QPen pen = painter->pen();
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setColor(Qt::black);
    rect = QRect(0, 320, painter->window().width(), int(painter->font().pixelSize()) * 6);
    pen.setColor(Qt::white);
    painter->setPen(pen);

    painter->drawText(rect.adjusted(0, 4, 4, 0), Qt::AlignHCenter, Prog::prepare);
    pen.setColor(Qt::gray);
    painter->setPen(pen);
    painter->drawText(rect.adjusted(0, 2, 2, 0), Qt::AlignHCenter, Prog::prepare);
    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->drawText(rect, Qt::AlignHCenter, Prog::prepare);
}
