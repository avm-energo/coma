#include "config85.h"

Config85::Config85(QVector<S2::DataRec> &config)
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
//        config.append({StartInIndex+7, sizeof(Bci_block.RESnom_OFF), Bci_block.RESnom_OFF});
//        config.append({StartInIndex+8, sizeof(Bci_block.RESnom_ON), Bci_block.RESnom_ON});
//        config.append({StartInIndex+9, sizeof(Bci_block.RESkz), &Bci_block.RESkz});
        config.append({StartInIndex+10, sizeof(Bci_block.RES_max), &Bci_block.RES_max});
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
        //...
        config.append({StartInIndex+61, sizeof(Bci_block.Adapt), &Bci_block.Adapt});
        config.append({StartInIndex+62, sizeof(Bci_block.TsolON), &Bci_block.TsolON});
        config.append({StartInIndex+63, sizeof(Bci_block.TsolOFF), &Bci_block.TsolOFF});
        config.append({StartInIndex+64, sizeof(Bci_block.NumA), &Bci_block.NumA});
        config.append({StartInIndex+65, sizeof(Bci_block.TypeA), &Bci_block.TypeA});
        //config.append({StartInIndex+66, sizeof(Bci_block.SolTime), &Bci_block.SolTime});
        config.append({StartInIndex+67, sizeof(Bci_block.RESnom_max), &Bci_block.RESnom_max});
        config.append({StartInIndex+68, sizeof(Bci_block.Pdrive_nom), &Bci_block.Pdrive_nom});
        config.append({StartInIndex+69, sizeof(Bci_block.Pdrive_pred), &Bci_block.Pdrive_pred});
        config.append({StartInIndex+70, sizeof(Bci_block.Pdrive_alarm), &Bci_block.Pdrive_alarm});
        config.append({StartInIndex+71, sizeof(Bci_block.dTs_OFF), &Bci_block.dTs_OFF});
        config.append({StartInIndex+72, sizeof(Bci_block.dTs_ON), &Bci_block.dTs_ON});
        //...
        config.append({StartInIndex+75, sizeof(Bci_block.RES_const_A_big), &Bci_block.RES_const_A_big});
        config.append({StartInIndex+76, sizeof(Bci_block.RES_const_a_small), &Bci_block.RES_const_a_small});
        config.append({StartInIndex+77, sizeof(Bci_block.RES_const_n_small), &Bci_block.RES_const_n_small});
        config.append({StartInIndex+78, sizeof(Bci_block.RES_const_b), &Bci_block.RES_const_b});
        config.append({StartInIndex+79, sizeof(Bci_block.RES_const_N_big), &Bci_block.RES_const_N_big});
        config.append({StartInIndex+80, sizeof(Bci_block.Algorithm_Choice), &Bci_block.Algorithm_Choice});
        config.append({StartInIndex+81, sizeof(Bci_block.Iotc), &Bci_block.Iotc});
        config.append({StartInIndex+83, sizeof(Bci_block.Tdis_OFF), &Bci_block.Tdis_OFF});
        config.append({StartInIndex+84, sizeof(Bci_block.Tdis_ON), &Bci_block.Tdis_ON});
        config.append({StartInIndex+90, sizeof(Bci_block.K_on_volt), &Bci_block.K_on_volt});
        config.append({StartInIndex+91, sizeof(Bci_block.K_off_volt), &Bci_block.K_off_volt});
        config.append({StartInIndex+92, sizeof(Bci_block.K_on_tamb), &Bci_block.K_on_tamb});
        config.append({StartInIndex+93, sizeof(Bci_block.K_off_tamb), &Bci_block.K_off_tamb});
        config.append({StartInIndex+94, sizeof(Bci_block.K_on_tdr), &Bci_block.K_on_tdr});
        config.append({StartInIndex+95, sizeof(Bci_block.K_off_tdr), &Bci_block.K_off_tdr});
        config.append({StartInIndex+96, sizeof(Bci_block.K_on_hdr), &Bci_block.K_on_hdr});
        config.append({StartInIndex+97, sizeof(Bci_block.K_off_hdr), &Bci_block.K_off_hdr});
        config.append({StartInIndex+98, sizeof(Bci_block.K_on_tbo), &Bci_block.K_on_tbo});
        config.append({StartInIndex+99, sizeof(Bci_block.K_off_tbo), &Bci_block.K_off_tbo});
        config.append({StartInIndex+100, sizeof(Bci_block.Kad), &Bci_block.Kad});
    }
    config.append({0xFFFFFFFF, 0, nullptr});
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
//        Bci_block.RESnom_OFF[i] = Bci_block.RESnom_ON[i] = Bci_block.RESkz[i]  = 0;
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
    Bci_block.Tzad_ON[0] = Bci_block.Tzad_OFF[0] = 0;
    Bci_block.Tzad_ON[1] = Bci_block.Tzad_OFF[1] = static_cast<float>(3.33);
    Bci_block.Tzad_ON[2] = Bci_block.Tzad_OFF[2] = static_cast<float>(6.67);
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
    Bci_block.dT_OFFpred = 2; 	// Погрешность выполненной синхронной коммутации при отключении (повышенное значение / предупредительная уставка)
    Bci_block.dT_OFFalarm = 3;		// Погрешность выполненной синхронной	коммутации при отключении (повышенное	значение / аварийная уставка)
    Bci_block.dT_ONpred = 2;	// Погрешность выполненной синхронной коммутации при включении (повышенное значение / предупредительная уставка)
    Bci_block.dT_ONalarm = 3;	// Погрешность выполненной синхронной коммутации при включении (повышенное значение / аварийная уставка)
    Bci_block.Tras_pred = 10;	// Время рассинхронизации фаз выключателя (повышенное значение / предупредительная уставка)
    Bci_block.Tras_alarm = 15;	// Время рассинхронизации фаз выключателя (повышенное значение / аварийная уставка)
    for (int i=0; i<2; ++i)
    {
        Bci_block.Tarc_pred[i] = 10;	// Время горения дуги в полюсе выключателя (повышенное значение / предупредительная уставка)
        Bci_block.Tarc_alarm[i] = 15;	// Время горения дуги в полюсе выключателя (повышенное значение / аварийная уставка)
    }
    Bci_block.RESnom_pred =100;	// Остаточный механический ресурс (пониженное значение / предупредительная уставка)
    Bci_block.RESnom_alarm =20;	 // Остаточный механический ресурс (пониженное значение / аварийная уставка)
    Bci_block.RESkz_pred = 5; 	// Остаточный коммутационный ресурс (пониженное значение / предупредительная уставка)
    Bci_block.RESkz_alarm =2; 		// Остаточный коммутационный ресурс (пониженное значение / аварийная уставка)
    Bci_block.I_pred = 3000; 	// Ток в выключателе при выполнении коммутации (повышенное значение / предупредительная уставка)
    Bci_block.I_alarm = 4000; 	// Ток в выключателе при выполнении коммутации (повышенное значение / аварийная уставка)
    Bci_block.Aux = 0;
    for (int i=0; i<2; ++i)
    {
        for (int j=0; j<2; ++j)
        {
            Bci_block.K_on_volt[i][j] = 0;
            Bci_block.K_off_volt[i][j] = 0;
            Bci_block.K_on_tamb[i][j] = 0;
            Bci_block.K_off_tamb[i][j] = 0;
            Bci_block.K_on_tdr[i][j] = 0;
            Bci_block.K_off_tdr[i][j] = 0;
            Bci_block.K_on_hdr[i][j] = 0;
            Bci_block.K_off_hdr[i][j] = 0;

        }
    }
    for (int i=0; i<3; ++i)
    {
        Bci_block.Tdis_OFF[i] = 0;  //дополнительное смещение момента отключения во избежании повторного зажигания, мс
        Bci_block.Tdis_ON[i] = 0; //дополнительное смещение момента включения для снижения перенапряжений, мс
    }
    Bci_block.Pdrive_nom = 32;
    Bci_block.Pdrive_pred = 33;
    Bci_block.Pdrive_alarm = 34;
    Bci_block.Adapt  = 0; 	// Адаптивный алгоритм не используется
    Bci_block.TsolOFF = 300;
    Bci_block.TsolON = 300;
    Bci_block.NumA = 1;
    Bci_block.TypeA = 0x01; // выключатель
    Bci_block.dTs_OFF = 1;   // допустимое отклонение собственного времени отключения, мс
    Bci_block.dTs_ON = 1;   // допустимое отклонение собственного времени включения, мс

    Bci_block.Algorithm_Choice = 1;
    Bci_block.Iotc = 20;
    Bci_block.RES_const_A_big = 3960;
    Bci_block.RES_const_N_big = 5000;
    Bci_block.RES_const_a_small = 0.63;
    Bci_block.RES_const_b = 9.68;
    Bci_block.RES_const_n_small = 2.0;
    Bci_block.K_on_tbo = 0.01;
    Bci_block.K_off_tbo = 0.01;
    Bci_block.Kad = 0.1;
}
