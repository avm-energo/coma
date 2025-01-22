#pragma once

#include <QComboBox>

class CBFunc
{
public:
    CBFunc();

    [[nodiscard]] static QComboBox *NewCB2(QWidget *parent, const QString &cbname, const QStringList &cbsl);
    [[nodiscard]] static QComboBox *NewCB2(QWidget *parent, const QStringList &cbsl);
    static bool SetCBData(const QWidget *parent, const QString &cbname, const QString &cbvalue);
    static bool SetCBIndex(const QObject *parent, const QString &cbname, int index);
    static bool SetCBColor(QWidget *parent, const QString &cbname, const QString &color);
    static QString CBData(const QWidget *parent, const QString &cbname);
    static int CBIndex(const QObject *parent, const QString &cbname);

    template <typename T> static T CBData(const QWidget *parent, const QString &cbname)
    {
        auto buffer = CBData(parent, cbname);
        if constexpr (std::is_floating_point_v<T>)
            buffer.replace(',', '.');
        return QVariant(buffer).value<T>();
    }
};
