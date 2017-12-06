#include "config85.h"

Config85::Config85(QVector<publicclass::DataRec> &config)
{
    // параметры входных сигналов
    quint32 StartInIndex = ID8085_START;
    for (int i=0; i<config.size(); ++i)
    {
        if (config.at(i).id == 0xFFFFFFFF)
        {
            config.remove(i);
            --i;
        }
    }
    if (StartInIndex != 0)
    {
        config.append({StartInIndex, sizeof(Bci_block.Eq_type), &Bci_block.Eq_type});
        config.append({StartInIndex+1, sizeof(Bci_block.Core_type), &Bci_block.Core_type});
        config.append({StartInIndex+2, sizeof(Bci_block.D_win), &Bci_block.D_win});
        config.append({StartInIndex+3, sizeof(Bci_block.N_Type), &Bci_block.N_Type});
        config.append({StartInIndex+4, sizeof(Bci_block.Op_type), &Bci_block.Op_type});
        config.append({StartInIndex+5, sizeof(Bci_block.Is_U2), &Bci_block.Is_U2});
        config.append({StartInIndex+6, sizeof(Bci_block.Residue), &Bci_block.Residue});
        config.append({StartInIndex+7, sizeof(Bci_block.RESnom_OFF), Bci_block.RESnom_OFF});
        config.append({StartInIndex+8, sizeof(Bci_block.RESnom_ON), Bci_block.RESnom_ON});
        config.append({StartInIndex+9, sizeof(Bci_block.RESkz), &Bci_block.RESkz});
        config.append({StartInIndex+10, sizeof(Bci_block.RESnom_max), &Bci_block.RESnom_max});
        config.append({StartInIndex+11, sizeof(Bci_block.RESkz_max), &Bci_block.RESkz_max});
        config.append({StartInIndex+12, sizeof(Bci_block.Unom), &Bci_block.Unom});
        config.append({StartInIndex+13, sizeof(Bci_block.Inom), &Bci_block.Inom});
        config.append({StartInIndex+14, sizeof(Bci_block.Ikz), &Bci_block.Ikz});
        config.append({StartInIndex+15, sizeof(Bci_block.ITT1nom), &Bci_block.ITT1nom});
        config.append({StartInIndex+16, sizeof(Bci_block.ITT2nom), &Bci_block.ITT2nom});
        config.append({StartInIndex+17, sizeof(Bci_block.T_arc), &Bci_block.T_arc});
        config.append({StartInIndex+18, sizeof(Bci_block.RDDS), &Bci_block.RDDS});
        config.append({StartInIndex+19, sizeof(Bci_block.RRDS), &Bci_block.RRDS});
        config.append({StartInIndex+20, sizeof(Bci_block.Ts_OFF), &Bci_block.Ts_OFF});
        config.append({StartInIndex+21, sizeof(Bci_block.Ts_ON), &Bci_block.Ts_ON});
        config.append({StartInIndex+22, sizeof(Bci_block.Tzad_OFF), &Bci_block.Tzad_OFF});
        config.append({StartInIndex+23, sizeof(Bci_block.Tzad_ON), &Bci_block.Tzad_ON});
        config.append({StartInIndex+24, sizeof(Bci_block.Tbk_OFF), &Bci_block.Tbk_OFF});
        config.append({StartInIndex+25, sizeof(Bci_block.Tbk_ON), &Bci_block.Tbk_ON});
        config.append({StartInIndex+26, sizeof(Bci_block.DUosc), &Bci_block.DUosc});
        config.append({StartInIndex+27, sizeof(Bci_block.DIosc), &Bci_block.DIosc});
        config.append({StartInIndex+28, sizeof(Bci_block.DUmin), &Bci_block.DUmin});
        config.append({StartInIndex+29, sizeof(Bci_block.DImin), &Bci_block.DImin});
        config.append({StartInIndex+30, sizeof(Bci_block.Ts_OFFpred), &Bci_block.Ts_OFFpred});
        config.append({StartInIndex+31, sizeof(Bci_block.Ts_OFFalarm), &Bci_block.Ts_OFFalarm});
        config.append({StartInIndex+32, sizeof(Bci_block.Ts_ONpred), &Bci_block.Ts_ONpred});
        config.append({StartInIndex+33, sizeof(Bci_block.Ts_ONalarm), &Bci_block.Ts_ONalarm});
        config.append({StartInIndex+34, sizeof(Bci_block.Tmain_OFFpred), &Bci_block.Tmain_OFFpred});
        config.append({StartInIndex+35, sizeof(Bci_block.Tmain_OFFalarm), &Bci_block.Tmain_OFFalarm});
        config.append({StartInIndex+36, sizeof(Bci_block.Tmain_ONpred), &Bci_block.Tmain_ONpred});
        config.append({StartInIndex+37, sizeof(Bci_block.Tmain_ONalarm), &Bci_block.Tmain_ONalarm});
        config.append({StartInIndex+38, sizeof(Bci_block.dT_OFFpred), &Bci_block.dT_OFFpred});
        config.append({StartInIndex+39, sizeof(Bci_block.dT_OFFalarm), &Bci_block.dT_OFFalarm});
        config.append({StartInIndex+40, sizeof(Bci_block.dT_ONpred), &Bci_block.dT_ONpred});
        config.append({StartInIndex+41, sizeof(Bci_block.dT_ONalarm), &Bci_block.dT_ONalarm});
        config.append({StartInIndex+42, sizeof(Bci_block.Tras_pred), &Bci_block.Tras_pred});
        config.append({StartInIndex+43, sizeof(Bci_block.Tras_alarm), &Bci_block.Tras_alarm});
        config.append({StartInIndex+44, sizeof(Bci_block.Tarc_pred), &Bci_block.Tarc_pred});
        config.append({StartInIndex+45, sizeof(Bci_block.Tarc_alarm), &Bci_block.Tarc_alarm});
        config.append({StartInIndex+46, sizeof(Bci_block.RESnom_pred), &Bci_block.RESnom_pred});
        config.append({StartInIndex+47, sizeof(Bci_block.RESnom_alarm), &Bci_block.RESnom_alarm});
        config.append({StartInIndex+48, sizeof(Bci_block.RESkz_pred), &Bci_block.RESkz_pred});
        config.append({StartInIndex+49, sizeof(Bci_block.RESkz_alarm), &Bci_block.RESkz_alarm});
        config.append({StartInIndex+50, sizeof(Bci_block.I_pred), &Bci_block.I_pred});
        config.append({StartInIndex+51, sizeof(Bci_block.I_alarm), &Bci_block.I_alarm});
        config.append({StartInIndex+52, sizeof(Bci_block.Aux), &Bci_block.Aux});
        config.append({StartInIndex+53, sizeof(Bci_block.K_on_volt), &Bci_block.K_on_volt});
        config.append({StartInIndex+54, sizeof(Bci_block.K_off_volt), &Bci_block.K_off_volt});
        config.append({StartInIndex+55, sizeof(Bci_block.K_on_tamb), &Bci_block.K_on_tamb});
        config.append({StartInIndex+56, sizeof(Bci_block.K_off_tamb), &Bci_block.K_off_tamb});
        config.append({StartInIndex+57, sizeof(Bci_block.K_on_tdr), &Bci_block.K_on_tdr});
        config.append({StartInIndex+58, sizeof(Bci_block.K_off_tdr), &Bci_block.K_off_tdr});
        config.append({StartInIndex+59, sizeof(Bci_block.K_on_hdr), &Bci_block.K_on_hdr});
        config.append({StartInIndex+60, sizeof(Bci_block.K_off_hdr), &Bci_block.K_off_hdr});
        config.append({StartInIndex+61, sizeof(Bci_block.Adapt), &Bci_block.Adapt});
        config.append({StartInIndex+62, sizeof(Bci_block.TsolON), &Bci_block.TsolON});
        config.append({StartInIndex+63, sizeof(Bci_block.TsolOFF), &Bci_block.TsolOFF});
        config.append({StartInIndex+64, sizeof(Bci_block.NumA), &Bci_block.NumA});
        config.append({StartInIndex+65, sizeof(Bci_block.TypeA), &Bci_block.TypeA});
    }
    config.append({0xFFFFFFFF, 0, NULL});
}

void Config85::SetDefConf()
{
    Bci_block.Eq_type = 0;	 // Тип коммутируемого оборудования: 0 – свободно конфигурируемый режим
    Bci_block.Core_type = 0;	// 0 – три однофазных сердечника
    Bci_block.D_win = 0;	// 0 – нет обмоток, соединенных в треугольник
    Bci_block.N_Type = 0;	// 0 – заземленная нейтраль
    Bci_block.Op_type = 0;	 // Тип обслуживаемых коммутационных операций: 0 – несинхронизируемые включение и отключение выключателя
    Bci_block.Is_U2 = 0;	// 0 – отсутствуют сигналы второй тройки напряжений
    Bci_block.Residue = 0;	// 0 – остаточный поток в трансформаторе не учитывается
    for (int i=0; i<3; ++i)
    {
        Bci_block.RESnom_OFF[i] = Bci_block.RESnom_ON[i] = Bci_block.RESkz[i]  = 0;
        Bci_block.Ts_OFF[i] = Bci_block.Ts_ON[i] = 20;
        Bci_block.Tbk_OFF[i] = Bci_block.Tbk_ON[i] = 1;
    }
    Bci_block.RESnom_max = 40000; // Предельное значение механического ресурса – 40000 операций
    Bci_block.RESkz_max = 16000.0;	// Предельное значение коммутационного ресурса – 16 кА2
    Bci_block.Unom = 220; 	// Номинальное первичное напряжение 220 кВ
    Bci_block.Inom = 2000; 	// Номинальный ток выключателя 2000 А
    Bci_block.Ikz = 6; 	// Номинальный ток выключателя 6 кА
    Bci_block.ITT1nom = 2000; 	 // Номинальный первичный ток  внешнего ТТ 2000А
    Bci_block.ITT2nom = 5; 	// Номинальные вторичные токи TT  -  5А
    Bci_block.T_arc = 5.0;     // минимальное время горения дуги – 5 мс
    Bci_block.RDDS = 70.0;	// скорость снижения электрической прочности контактов при включении – 70 кВ/мс
    Bci_block.RRDS = 70.0;	// скорость роста электрической прочности контактов при включении – 70 кВ/мс
    Bci_block.Tzad_OFF[0] = Bci_block.Tzad_OFF[0] = 0;
    Bci_block.Tzad_OFF[1] = Bci_block.Tzad_OFF[1] = static_cast<float>(3.33);
    Bci_block.Tzad_OFF[2] = Bci_block.Tzad_OFF[2] = static_cast<float>(6.67);
    Bci_block.DUosc = 10;         // уставка скачка напряжения 10%
    Bci_block.DIosc = 20;           // уставка скачка тока 20%
    Bci_block.DUmin =10;         // уставка минимума напряжения 10%
    Bci_block.DImin =2;            // уставка минимума тока 2%
    Bci_block.Ts_OFFpred=25; // Собственное время отключения полюса выключателя
    Bci_block.Ts_OFFalarm=30; // Собственное время отключения полюса выключателя
    Bci_block.Ts_ONpred=25; // Собственное время включения полюса выключателя
    Bci_block.Ts_ONalarm=30; // Собственное время включения полюса
    Bci_block.Tmain_OFFpred=25;	// Время перемещения главного контакта при отключении полюса выключателя
    Bci_block.Tmain_OFFalarm=30; // Время перемещения главного контакта при отключении полюса выключателя
    Bci_block.Tmain_ONpred=25;	// Время перемещения главного контакта при включении полюса выключателя (повышенное значение / предупредительная уставка)
    Bci_block.Tmain_ONalarm=30;   // Время перемещения главного контакта при включении полюса выключателя (повышенное значение / аварийная уставка)
    Bci_block.dT_OFFpred =2; 	// Погрешность выполненной синхронной коммутации при отключении (повышенное значение / предупредительная уставка)
    Bci_block.dT_OFFalarm =3;		// Погрешность выполненной синхронной	коммутации при отключении (повышенное	значение / аварийная уставка)
    Bci_block.dT_ONpred =2;	// Погрешность выполненной синхронной коммутации при включении (повышенное значение / предупредительная уставка)
    Bci_block.dT_ONalarm =3;	// Погрешность выполненной синхронной коммутации при включении (повышенное значение / аварийная уставка)
    Bci_block.Tras_pred = 10;	// Время рассинхронизации фаз выключателя (повышенное значение / предупредительная уставка)
    Bci_block.Tras_alarm = 15;	// Время рассинхронизации фаз выключателя (повышенное значение / аварийная уставка)
    Bci_block.Tarc_pred = 10;	// Время горения дуги в полюсе выключателя (повышенное значение / предупредительная уставка)
    Bci_block.Tarc_alarm = 15;	// Время горения дуги в полюсе выключателя (повышенное значение / аварийная уставка)
    Bci_block.RESnom_pred =100;	// Остаточный механический ресурс (пониженное значение / предупредительная уставка)
    Bci_block.RESnom_alarm =20;	 // Остаточный механический ресурс (пониженное значение / аварийная уставка)
    Bci_block.RESkz_pred = 5; 	// Остаточный коммутационный ресурс (пониженное значение / предупредительная уставка)
    Bci_block.RESkz_alarm =2; 		// Остаточный коммутационный ресурс (пониженное значение / аварийная уставка)
    Bci_block.I_pred = 3000; 	// Ток в выключателе при выполнении коммутации (повышенное значение / предупредительная уставка)
    Bci_block.I_alarm = 4000; 	// Ток в выключателе при выполнении коммутации (повышенное значение / аварийная уставка)
    Bci_block.Aux = 0;		// вспомогательные аналоговые сигналы не используются
    Bci_block.K_on_volt = 0;
    Bci_block.K_off_volt = 0;
    Bci_block.K_on_tamb = 0;
    Bci_block.K_off_tamb = 0;
    Bci_block.K_on_tdr = 0;
    Bci_block.K_off_tdr = 0;
    Bci_block.K_on_hdr = 0;
    Bci_block.K_off_hdr = 0;
    Bci_block.Adapt  = 0; 	// Адаптивный алгоритм не используется
    Bci_block.TsolOFF = 300;
    Bci_block.TsolON = 300;
    Bci_block.NumA = 1;
    Bci_block.TypeA = 0x01; // выключатель
}
