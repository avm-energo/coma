#include "ledindicator.h"

LedIndicator::LedIndicator(QWidget *parent) : QWidget(parent)
{
}

void LedIndicator::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    this->height = this->size().height();
    this->width = this->size().width();
    this->minDim = (height > width) ? width : height;
    this->half = minDim / 2;
    this->centerX = width / 2;
    this->centerY = height / 2;

    this->outerBorderWidth = minDim / 10;
    this->innerBorderWidth = minDim / 14;
    this->outerBorderRadius = half - outerBorderWidth;
    this->innerBorderRadius = half - (outerBorderWidth + innerBorderWidth);

    this->topReflexY = centerY - (half - outerBorderWidth - innerBorderWidth) / 2;
    this->bottomReflexY = centerY + (half - outerBorderWidth - innerBorderWidth) / 2;
    this->topReflexHeight = half / 5;
    this->topReflexWidth = half / 3;
    this->bottomReflexHeight = half / 5;
    this->bottomReflexWidth = half / 3;

    drawingRect.setTop((height - minDim) / 2);
    drawingRect.setLeft((width - minDim) / 2);
    drawingRect.setHeight(minDim);
    drawingRect.setWidth(minDim);
}

void LedIndicator::paintEvent(QPaintEvent *event)
{
    QColor ledColor(Qt::green);
    QWidget::paintEvent(event);
    this->drawLed(ledColor);
}

void LedIndicator::drawLed(const QColor &color)
{

    QColor ledColor(Qt::green);
    QColor lightColor(QColor(0xE0, 0xE0, 0xE0));
    QColor shadowColor(QColor(0x70, 0x70, 0x70));
    QColor ringShadowDarkColor(QColor(0x50, 0x50, 0x50, 0xFF));
    QColor ringShadowMedColor(QColor(0x50, 0x50, 0x50, 0x20));

    QColor ringShadowLightColor(QColor(0xEE, 0xEE, 0xEE, 0x00));
    QColor topReflexUpColor(QColor(0xFF, 0xFF, 0xFF, 0xA0));
    QColor topReflexDownColor(QColor(0xFF, 0xFF, 0xFF, 0x00));
    QColor bottomReflexCenterColor(QColor(0xFF, 0xFF, 0xFF, 0x00));
    QColor bottomReflexSideColor(QColor(0xFF, 0xFF, 0xFF, 0x70));
    QPainter p(this);

    QPen pen;
    pen.setStyle(Qt::NoPen);
    p.setPen(pen);

    QRadialGradient outerRingGradient(
        QPoint(centerX, centerY - outerBorderRadius - (outerBorderWidth / 2)), minDim - (outerBorderWidth / 2));
    outerRingGradient.setColorAt(0, lightColor);
    outerRingGradient.setColorAt(1, shadowColor);
    QBrush outerRingBrush(outerRingGradient);
    p.setBrush(outerRingBrush);
    p.drawEllipse(this->drawingRect);
}
