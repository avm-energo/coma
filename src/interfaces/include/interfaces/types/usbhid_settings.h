#pragma once

#include <avm-gen/pch.h>
#include <interfaces/types/base_settings.h>

#include <QDebug>

class UsbHidSettings final : public BaseSettings
{
    Q_OBJECT
public:
    explicit UsbHidSettings(QObject *parent = nullptr);
    //     unsigned short vendor_id;
    //     unsigned short product_id;
    //     QString serial;
    // #ifdef QT_DEBUG
    //     QString path;
    // #endif

    explicit UsbHidSettings(unsigned short dev, unsigned short pid, const QString &arr, const QString &str = "");

    // bool hasMatch(const QString &str) const;
    // bool hasPartialMatch(const QString &str) const;

    friend bool operator==(UsbHidSettings &lhs, UsbHidSettings &rhs);
    friend bool operator!=(UsbHidSettings &lhs, UsbHidSettings &rhs);
    friend QDebug operator<<(QDebug debug, UsbHidSettings *settings);
};
// Q_DECLARE_METATYPE(UsbHidSettings)
