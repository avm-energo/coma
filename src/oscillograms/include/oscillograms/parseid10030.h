#pragma once

#include <oscillograms/parsemodule.h>

class ParseID10030 : public ParseModule
{
public:
    ParseID10030(const QByteArray &BA);

    struct Point85
    {
        float An[9]; // Ua,Ub,Uc (напряжение источника), Ia, Ib, Ic (ток ВВ), Ua,Ub,Uc (напряжение нагрузки)
        quint32 Dis;
    };

    // Dis:
    // 31-29:	nOx – НО блок-контакты полюсов А, В, С (нефильтр.)
    // 28-26:	nCx – НЗ блок-контакты полюсов А, В, С (нефильтр.)
    // 25-23:	nFx – оперативное управление отключением полюсов А, В, С (нефильтр.)
    // 22-20:	nNx – оперативное управление включением полюсов А, В, С (нефильтр.)
    // 19-17:	CFx – выходные команды на отключение полюсов А, В, С
    // 16-14:	CNx – выходные команды на включение полюсов А, В, С
    // 13:      CSC – контроль цепи соленоида отключения
    // 12:      CSO – контроль цепи соленоида включения
    // 11-9:	BKOx – НО блок-контакты полюсов А, В, С
    // 8-6:     BKCx – НЗ блок-контакты полюсов А, В, С
    // 5-3:     OCFx – оперативное управление отключением полюсов А, В, С
    // 2-0:     OCNx – оперативное управление включением полюсов А, В, С

public:
    bool Parse(quint32 id, const S2::OscHeader &header, TrendViewModel *model) override;
};
