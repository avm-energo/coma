#pragma once

#include "parsemodule.h"

class ParseID10023 final : public ParseModule
{
private:
    struct DataPoint
    {
        float U; ///< напряжения в кВ
        float I; ///< токи в мА
    };

public:
    explicit ParseID10023(const QByteArray &ba);
    bool Parse(quint32 id, const S2::OscHeader &header, TrendViewModel *model) override;
};
