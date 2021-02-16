#include "aboutwidget.h"

#include "wd_func.h"

#include <QPainter>
#include <QPixmap>
#include <QVBoxLayout>

AboutWidget::AboutWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *lyout = new QVBoxLayout;

    hlyout->addWidget(WDFunc::NewLBL2(this, "", "", new QPixmap("images/avm-energo.png")), 0);
    lyout->addWidget(WDFunc::NewLBL2(this, AboutText::name_ru));
    lyout->addWidget(WDFunc::NewLBL2(this, AboutText::department));
    lyout->addSpacing(30);
    lyout->addWidget(WDFunc::NewLBL2(this, AboutText::modules));
    lyout->addWidget(WDFunc::NewLBL2(this, AboutText::zlib));
    lyout->addWidget(WDFunc::NewLBL2(this, AboutText::hidapi));
    lyout->addWidget(WDFunc::NewLBL2(this, AboutText::qxlsx));
    lyout->addWidget(WDFunc::NewLBL2(this, AboutText::limereport));
    lyout->addWidget(WDFunc::NewLBL2(this, AboutText::icons));
    lyout->addWidget(WDFunc::NewLBL2(this, AboutText::fonts));
    hlyout->addLayout(lyout, 100);
    lyout = new QVBoxLayout;
    lyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addStretch(100);
    hlyout->addWidget(WDFunc::NewHexagonPB(
        this, "", [this]() { this->close(); }, ":/icons/tnyes.svg", "Закрыть"));
    hlyout->addStretch(100);
    lyout->addLayout(hlyout);
    setLayout(lyout);
}

// void AboutWidget::paintEvent(QPaintEvent *e)
//{
//    QPainter painter(this);
//    const int margin = 20;
//    QFont font = painter.font();
//    font.setPixelSize(16);
//    painter.setFont(font);

//    QRect rect(margin, margin, painter.window().width() - 2 * margin, painter.font().pixelSize() * 1.2);
//    QString name;
//    name.append(AboutText::name_ru).append(" (").append(PROGNAME).append(") ").append(COMAVERSION);

//    painter.drawText(rect, Qt::AlignRight | Qt::AlignVCenter, name);
//    rect.moveBottom(rect.bottom() + 2 * painter.font().pixelSize());
//    painter.drawText(rect, Qt::AlignRight | Qt::AlignVCenter, AboutText::department);

//    font.setPixelSize(12);
//    painter.setFont(font);
//    rect.setHeight(painter.font().pixelSize() * 1.2);

//    rect.moveBottom(rect.bottom() + 1.7 * painter.font().pixelSize());
//    painter.drawText(rect, Qt::AlignLeft, AboutText::modules);

//    rect.moveBottom(rect.bottom() + 1.7 * painter.font().pixelSize());
//    painter.drawText(rect, Qt::AlignLeft, AboutText::zlib);

//    rect.moveBottom(rect.bottom() + 1.7 * painter.font().pixelSize());
//    painter.drawText(rect, Qt::AlignLeft, AboutText::hidapi);

//    rect.moveBottom(rect.bottom() + 1.7 * painter.font().pixelSize());
//    painter.drawText(rect, Qt::AlignLeft, AboutText::qxlsx);

//    rect.moveBottom(rect.bottom() + 1.7 * painter.font().pixelSize());
//    painter.drawText(rect, Qt::AlignLeft, AboutText::limereport);

//    rect.moveBottom(rect.bottom() + 1.7 * painter.font().pixelSize());
//    painter.drawText(rect, Qt::AlignLeft, AboutText::icons);

//    rect.moveBottom(rect.bottom() + 1.7 * painter.font().pixelSize());
//    painter.drawText(rect, Qt::AlignLeft, AboutText::fonts);

//    rect.moveBottom(rect.bottom() + 1.7 * painter.font().pixelSize());

//    e->accept();
//}
