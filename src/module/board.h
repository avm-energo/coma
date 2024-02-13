#pragma once

#define DEVICETYPE_MODULE 1 // модуль
#define DEVICETYPE_DEVICE 2 // прибор

#include "modules.h"

#include <gen/datatypes.h>
#include <gen/singleton.h>
#include <interfaces/types/common_types.h>

class Board : public QObject, public Singleton<Board>
{
    Q_OBJECT

public:
    // TODO: Здесь мог бы быть ваш рефакторинг ;)
    enum Types
    {
        None = 0,
        Base = 1,
        Mezzanine = 2,
        BaseMezz = 3,
        Additional = 4,
        BaseAdd = 5,
        MezzAdd = 6,
        BaseMezzAdd = 7
    };

    Q_ENUM(Types)

    explicit Board(token);

    quint16 typeB() const;
    quint16 typeM() const;

    quint32 serialNumber(Types type) const;
    QString UID() const;
    bool noConfig() const;

    const Modules::StartupInfoBlock &baseSerialInfo() const
    {
        return m_startupInfoBlock;
    }

    const Modules::StartupInfoBlockExt0 &baseSerialInfoExt() const
    {
        return m_startupInfoBlockExt;
    }

private:
    Modules::StartupInfoBlock m_startupInfoBlock {};
    Modules::StartupInfoBlockExt0 m_startupInfoBlockExt {};
    int m_updateCounter = 0;
    int m_updateCounterExt = 0;
    bool m_updateType = false;

    void updateExt(const DataTypes::BitStringStruct &bs);
};
