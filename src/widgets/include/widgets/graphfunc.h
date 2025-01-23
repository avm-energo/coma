#pragma once

#include <QLabel>

class GraphFunc
{
public:
    GraphFunc();

    [[nodiscard]] static QLabel *NewIcon(QWidget *parent, const QString &iconpath = "");
    [[nodiscard]] static QPixmap NewCircle(QColor color, int radius);
    [[nodiscard]] static QPixmap NewLedIndicator(QColor color, float height);
    [[nodiscard]] static QPixmap NewSVG(QString &str, QSize size);
    [[nodiscard]] static QFrame *newHLine(QWidget *parent);
    [[nodiscard]] static QFrame *newVLine(QWidget *parent);
};
