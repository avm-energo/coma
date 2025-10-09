#pragma once

#include <interfaces/types/base_settings.h>

#include <QStringList>

// struct IEC104ConnectionParams
// {
//     std::uint8_t t0 = 30;
//     std::uint8_t t1 = 15;
//     std::uint8_t t2 = 10; ///< Тайм-аут для подтверждения в случае отсутствия сообщения с данными.
//     std::uint8_t t3 = 20; ///< Тайм-аут для посылки блоков тестирования в случае долгого простоя.
//     std::uint16_t k = 12; ///< Максимальное число неподтверждённых APDU, отправляемых станцией.
//     std::uint16_t w = 8; ///< После приёма w APDU формата I требуется подтверждение.
// };

class IEC104Settings final : public BaseSettings
{
    Q_OBJECT
    // QString ip;
    // std::uint16_t port;
    // std::uint16_t bsAddress;
    // IEC104ConnectionParams params;
public:
    explicit IEC104Settings(QObject *parent = nullptr) noexcept;

    bool isValid() const;
};
