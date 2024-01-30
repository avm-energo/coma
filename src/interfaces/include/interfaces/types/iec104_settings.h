#pragma once

#include <QStringList>
#include <interfaces/types/base_settings.h>

struct IEC104ConnectionParams
{
    std::uint8_t t0 = 30; ///< Тайм-аут при установке соединения.
    std::uint8_t t1 = 15; ///< Тайм-аут при посылке или тестировании APDU.
    std::uint8_t t2 = 10; ///< Тайм-аут для подтверждения в случае отсутствия сообщения с данными.
    std::uint8_t t3 = 20; ///< Тайм-аут для посылки блоков тестирования в случае долгого простоя.
    std::uint16_t k = 12; ///< Максимальное число неподтверждённых APDU, отправляемых станцией.
    std::uint16_t w = 8; ///< После приёма w APDU формата I требуется подтверждение.
};

struct IEC104Settings final : public BaseSettings
{
    QString ip;
    std::uint16_t port;
    std::uint16_t bsAddress;
    IEC104ConnectionParams params;

    bool isValid() const
    {
        auto parts = ip.split(".");
        bool ok = true;
        for (const auto &part : parts)
            if ((part.toUInt(&ok) >= 256) || !ok)
                return false;
        return ok;
    }
};
