#include "config85.h"

Config85::Config85(QVector<publicclass::DataRec> &config)
{
    // параметры входных сигналов
    quint32 StartInIndex = ID8085_START;
    publicclass::DataRec configelement;
    bool FFound = false;
    foreach (configelement, config)
    {
        if (configelement.id == 0xFFFFFFFF)
        {
            FFound = true;
            break;
        }
    }
    if (!FFound)
        config.append({0xFFFFFFFF, 0, NULL});
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
    }
}

void Config85::SetDefConf()
{

}
