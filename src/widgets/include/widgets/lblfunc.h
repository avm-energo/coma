#pragma once

#include <QLabel>

class LBLFunc
{
public:
    LBLFunc();

    static bool SetLBLTColor(QWidget *parent, const QString &lblname, const QString &color);
    [[nodiscard]] static QLabel *NewLBL2(QWidget *parent, const QString &text, const QString &lblname = "",
        const QPixmap *pm = nullptr, const QString &lbltip = "");
    [[nodiscard]] static QLabel *NewLBLT2(QWidget *parent, const QString &text, const QString &lblname = "",
        const QString &lbltip = "", bool fixed = false);

    static bool SetLBLImage(QWidget *parent, const QString &lblname, QPixmap *pm);
    static bool SetLBLColor(QWidget *parent, const QString &lblname, const QString &lblcolor);
    static bool SetLBLText(QWidget *parent, const QString &lblname, const QString &lbltext = "", bool enabled = true);
    [[nodiscard]] static QString LBLText(QWidget *parent, const QString &lblname);
    [[nodiscard]] static QWidget *NewLBLAndLBL(QWidget *parent, QString lblname, QString caption, bool enabled = false);
};
