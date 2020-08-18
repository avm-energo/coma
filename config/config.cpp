// config.cpp
#include "config.h"

#include "../gen/modulebsi.h"

Config::Config(QVector<S2::DataRec> *config, quint32 MTypeB, quint32 MTypeM)
{
    MainBlk.MTypeB = MTypeB;
    MainBlk.MTypeM = MTypeM;

    // общая часть
    config->append({ BCI_MTYPEB, sizeof(MainBlk.MTypeB), &MainBlk.MTypeB });
    config->append({ BCI_MTYPEM, sizeof(MainBlk.MTypeM), &MainBlk.MTypeM });
    config->append({ BCI_CTYPE, sizeof(MainBlk.Ctype), &MainBlk.Ctype });
    config->append({ BCI_ABS_104, sizeof(MainBlk.Abs_104), &MainBlk.Abs_104 });
    config->append({ BCI_CYCLE_104, sizeof(MainBlk.Cycle_104), &MainBlk.Cycle_104 });
    config->append({ BCI_T1_104, sizeof(MainBlk.T1_104), &MainBlk.T1_104 });
    config->append({ BCI_T2_104, sizeof(MainBlk.T2_104), &MainBlk.T2_104 });
    config->append({ BCI_T3_104, sizeof(MainBlk.T3_104), &MainBlk.T3_104 });
    config->append({ BCI_K_104, sizeof(MainBlk.k_104), &MainBlk.k_104 });
    config->append({ BCI_W_104, sizeof(MainBlk.w_104), &MainBlk.w_104 });

    //    config->append({ BCI_IP, sizeof(Com_param.IP), &Com_param.IP });
    //    config->append({ BCI_MASK, sizeof(Com_param.Mask), &Com_param.Mask });
    //    config->append({ BCI_GW, sizeof(Com_param.GateWay), &Com_param.GateWay });
    //    config->append({ BCI_PORT, sizeof(Com_param.Port), &Com_param.Port });
    //    config->append({ BCI_SNTP, sizeof(Com_param.SNTP), &Com_param.SNTP });
    //    config->append({ BCI_BAUD, sizeof(Com_param.Baud), &Com_param.Baud });
    //    config->append({ BCI_PARITY, sizeof(Com_param.Parity), &Com_param.Parity });
    //    config->append({ BCI_STOPBIT, sizeof(Com_param.Stopbit), &Com_param.Stopbit });
    //    config->append({ BCI_ADRMB, sizeof(Com_param.adrMB), &Com_param.adrMB });
}

void Config::SetDefBlock()
{
    MainBlk.MTypeB = DEF_MTYPEB;
    MainBlk.MTypeM = DEF_MTYPEM;
    MainBlk.Ctype = DEF_CTYPE;
    MainBlk.Abs_104 = DEF_ABS_104;
    MainBlk.Cycle_104 = DEF_CYCLE_104;
    MainBlk.T1_104 = DEF_T1_104;
    MainBlk.T2_104 = DEF_T2_104;
    MainBlk.T3_104 = DEF_T3_104;
    MainBlk.k_104 = DEF_K_104;
    MainBlk.w_104 = DEF_W_104;
}
