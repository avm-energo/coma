#include <interfaces/types/iec104_settings.h>

IEC104Settings::IEC104Settings(QObject *parent) noexcept : BaseSettings(parent)
{
    set("ip", "0.0.0.0");
    set("port", 0);
    set("bsAdress", 0);
    set("t0", 30); ///< Тайм-аут при установке соединения.
    set("t1", 15); ///< Тайм-аут при посылке или тестировании APDU.
    set("t2", 10); ///< Тайм-аут для подтверждения в случае отсутствия сообщения с данными.
    set("t3", 20); ///< Тайм-аут для посылки блоков тестирования в случае долгого простоя.
    set("k", 12);  ///< Максимальное число неподтверждённых APDU, отправляемых станцией.
    set("w", 8);   ///< После приёма w APDU формата I требуется подтверждение.
}

bool IEC104Settings::isValid() const
{
    auto parts = QString(get("ip")).split(".");
    bool ok = true;
    for (const auto &part : parts)
        if ((part.toUInt(&ok) >= 256) || !ok)
            return false;
    return ok;
}
