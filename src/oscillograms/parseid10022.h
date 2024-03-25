#pragma once

#include "parsemodule.h"

#include <array>

class ParseID10022 final : public ParseModule
{
private:
    struct DataPoint
    {
        std::array<float, 6> data;
        // float U[3]; ///< напряжения в кВ
        // float I[3]; ///< токи в мА
    };

public:
    explicit ParseID10022(const QByteArray &ba);
    bool Parse(quint32 id, const S2::OscHeader &header, TrendViewModel *model) override;
};
