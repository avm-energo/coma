#pragma once

#include <gen/pch.h>

#include <QDebug>
#include <QTableView>

class QMainWindow;
class QStatusBar;
class QWidget;
class UDialog;

class WDFunc
{
public:
    WDFunc();
    // static bool SetCWData(QWidget *parent, const QString &cwname, const QString &cwvalue);
    // [[nodiscard]] static QString CWData(QWidget *parent, const QString &cwname);
    // static bool SetCWColor(QWidget *parent, const QString &cwname, const QColor &color);

    [[nodiscard]] static QLabel *NewIcon(QWidget *parent, const QString &iconpath = "");

    static void SetEnabled(QWidget *w, const QString &wname, bool enabled);
    static void SetVisible(QWidget *w, const QString &wname, bool visible);
    static void setMinimumSize(QWidget *w, const QString &wname, int width, int height);
    [[nodiscard]] static QString StringFloatValueWithCheck(float value, int precision = 5, bool exp = false);
    [[nodiscard]] static QVariant FloatValueWithCheck(float value);

    template <typename T> //
    inline static T *GetChildFromParents(const QList<QWidget *> &parents, const QString &name)
    {
        for (auto &parent : parents)
        {
            auto child = parent->findChild<T *>(name);
            if (child != nullptr)
                return child;
        }
        return nullptr;
    }

    [[nodiscard]] static QStatusBar *NewSB(QWidget *parent);
    [[nodiscard]] static QPixmap NewCircle(QColor color, int radius);
    [[nodiscard]] static QPixmap NewLedIndicator(QColor color, float height);
    [[nodiscard]] static QPixmap NewSVG(QString &str, QSize size);
    [[nodiscard]] static QFrame *newHLine(QWidget *parent);
    [[nodiscard]] static QFrame *newVLine(QWidget *parent);

    [[nodiscard]] static QMainWindow *getMainWindow();

    static bool floatIsWithinLimits(
        const QString &varname, double var, double base, double tolerance, bool showMessage = true);
    [[nodiscard]] static QString ChooseFileForOpen(QWidget *parent, QString mask);
    /// Input: QString mask: описание файлов, например: "Файлы журналов (*.swj)";
    /// QString ext - расширение по умолчанию Output: QString filename
    [[nodiscard]] static QString ChooseFileForSave(
        QWidget *parent, const QString &mask, const QString &ext, const QString &filename);
    [[nodiscard]] static QString ChooseFileForSave(UDialog *parent, const QString &mask, const QString &ext);
    [[nodiscard]] static QString ChooseDirectoryForOpen(QWidget *parent);
};
