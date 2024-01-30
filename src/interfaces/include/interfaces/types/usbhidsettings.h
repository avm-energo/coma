#pragma once

#include <QDebug>
#include <gen/pch.h>
#include <interfaces/types/base_settings.h>

struct UsbHidSettings final : public BaseSettings
{
    unsigned short vendor_id;
    unsigned short product_id;
    QString serial;
#ifdef QT_DEBUG
    QString path;
#endif

    explicit UsbHidSettings() noexcept = default;
#ifdef QT_DEBUG
    explicit UsbHidSettings(unsigned short dev, unsigned short pid, const QString &arr, const QString &str);
#else
    explicit UsbHidSettings(unsigned short dev, unsigned short pid, const QString &arr);
#endif

    bool hasMatch(const QString &str) const;
    bool hasPartialMatch(const QString &str) const;

    friend bool operator==(const UsbHidSettings &lhs, const UsbHidSettings &rhs);
    friend bool operator!=(const UsbHidSettings &lhs, const UsbHidSettings &rhs);
    friend QDebug operator<<(QDebug debug, const UsbHidSettings &settings);
};
Q_DECLARE_METATYPE(UsbHidSettings)
