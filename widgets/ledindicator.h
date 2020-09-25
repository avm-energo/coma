#ifndef LEDINDICATOR_H
#define LEDINDICATOR_H

#include <QBrush>
#include <QColor>
#include <QGradient>
#include <QPainter>
#include <QPen>
#include <QResizeEvent>
#include <QWidget>

class LedIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit LedIndicator(QWidget *parent = nullptr);

private:
    int height;
    int width;
    int minDim;
    int half;
    int centerX;
    int centerY;
    QRect drawingRect;

    int outerBorderWidth;
    int innerBorderWidth;
    int outerBorderRadius;
    int innerBorderRadius;
    int topReflexY;
    int bottomReflexY;
    int topReflexWidth;
    int topReflexHeight;
    int bottomReflexWidth;
    int bottomReflexHeight;
    void drawLed(const QColor &color);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
};

#endif // LEDINDICATOR_H
