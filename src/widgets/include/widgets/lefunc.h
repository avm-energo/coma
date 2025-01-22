#pragma once

#include <QHBoxLayout>
#include <QLineEdit>

class PasswordLineEdit;
class LEFunc
{
public:
    LEFunc();

    [[nodiscard]] static QLineEdit *NewLE2(
        QWidget *parent, const QString &lename, const QString &letext = "", const QString &tooltip = "");
    [[nodiscard]] static PasswordLineEdit *NewPswLE2(
        QWidget *parent, const QString &lename, QLineEdit::EchoMode echostyle = QLineEdit::Normal);
    static bool SetLEData(QObject *parent, const QString &lename, const QString &levalue, const QString &restring = "");
    [[nodiscard]] static QString LEData(QObject *parent, const QString &lename);

    template <typename T> static void LEData(QObject *parent, const QString &lename, T &value)
    {
        auto le = parent->findChild<QLineEdit *>(lename);
        if (le == nullptr)
            value = T();
        else
            value = qvariant_cast<T>(le->text());
    }

    template <typename T> static bool LENumber(QWidget *parent, const QString &lename, T &levalue)
    {
        auto le = parent->findChild<QLineEdit *>(lename);
        if (le == nullptr)
            return false;
        levalue = le->text().toDouble();
        return true;
    }

    static bool SetLEColor(QWidget *parent, const QString &lename, const QColor &color);

    [[nodiscard]] static QWidget *NewLBLAndLE(QWidget *parent, QString caption, QString lename, bool enabled = false);
};
