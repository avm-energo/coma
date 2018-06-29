// config.cpp
#include "config.h"
//#include "../gen/publicclass.h"
#include "../gen/modulebsi.h"

Config::Config(QVector<S2::DataRec> &config)
{
    // общая часть
    config.append({BCI_CTYPE, sizeof(MainBlk.Ctype), &MainBlk.Ctype});
    config.append({BCI_ABS_104, sizeof(MainBlk.Abs_104), &MainBlk.Abs_104});
    config.append({BCI_CYCLE_104, sizeof(MainBlk.Cycle_104), &MainBlk.Cycle_104});
    config.append({BCI_T1_104, sizeof(MainBlk.T1_104), &MainBlk.T1_104});
    config.append({BCI_T2_104, sizeof(MainBlk.T2_104), &MainBlk.T2_104});
    config.append({BCI_T3_104, sizeof(MainBlk.T3_104), &MainBlk.T3_104});
    config.append({BCI_K_104, sizeof(MainBlk.k_104), &MainBlk.k_104});
    config.append({BCI_W_104, sizeof(MainBlk.w_104), &MainBlk.w_104});
}

void Config::SetDefBlock()
{
    MainBlk.Ctype = DEF_CTYPE;
    MainBlk.Abs_104 = DEF_ABS_104;
    MainBlk.Cycle_104 = DEF_CYCLE_104;
    MainBlk.T1_104 = DEF_T1_104;
    MainBlk.T2_104 = DEF_T2_104;
    MainBlk.T3_104 = DEF_T3_104;
    MainBlk.k_104 = DEF_K_104;
    MainBlk.w_104 = DEF_W_104;
}
