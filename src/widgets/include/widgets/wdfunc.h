#pragma once

#include <QString>

class QMainWindow;
class QWidget;
class WDFunc
{
public:
    WDFunc();
    static void SetEnabled(QWidget *w, const QString &wname, bool enabled);
    static void SetVisible(QWidget *w, const QString &wname, bool visible);
    static bool floatIsWithinLimits(const QString &varname, double var, double base, double tolerance);
    [[nodiscard]] static QString StringFloatValueWithCheck(float value, int precision = 5, bool exp = false);
    [[nodiscard]] static QMainWindow *getMainWindow();
};
