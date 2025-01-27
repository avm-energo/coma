#include <gen/stdfunc.h>
#include <widgets/epopup.h>
#include <widgets/wdfunc.h>
#include <gen/pch.h>

#include <QApplication>
#include <QMainWindow>

QMainWindow *WDFunc::getMainWindow()
{
    const auto widgets = qApp->topLevelWidgets();
    for (auto parent : widgets)
    {
        auto mainWin = qobject_cast<QMainWindow *>(parent);
        if (mainWin != nullptr)
            return mainWin;
    }
    return nullptr;
}

void WDFunc::SetEnabled(QWidget *parent, const QString &wname, bool enabled)
{
    auto widget = parent->findChild<QWidget *>(wname);
    if (widget != nullptr)
        widget->setEnabled(enabled);
}

void WDFunc::SetVisible(QWidget *parent, const QString &wname, bool visible)
{
    auto widget = parent->findChild<QWidget *>(wname);
    if (widget != nullptr)
    {
        if (visible)
            widget->show();
        else
            widget->hide();
    }
    else
        qDebug() << "No such widget to set it visible";
}

bool WDFunc::floatIsWithinLimits(const QString &varname, double var, double base, double tolerance)
{
    if (StdFunc::FloatIsWithinLimits(var, base, tolerance))
        return true;
    else
    {
        qCritical() << "Ошибочное значение " + varname + ": должно быть " << QString::number(base, 'f', 5) << "±"
                    << QString::number(tolerance, 'f', 5) << ", а получили: " << QString::number(var, 'f', 5);
        EMessageBox::error(nullptr,
            "Ошибочное значение " + varname + ": должно быть " + QString::number(base, 'f', 5) + "±"
                + QString::number(tolerance, 'f', 5) + ", а получили: " + QString::number(var, 'f', 5));
    }
    return false;
}

QString WDFunc::StringFloatValueWithCheck(float value, int precision, bool exp)
{
    if (value >= MAXFLOAT || value <= -MAXFLOAT)
        return "***";
    else if (exp == true)
        return QString::number(value, 'e', precision);
    else
        return QString::number(value, 'f', precision);
}
