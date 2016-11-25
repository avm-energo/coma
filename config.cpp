/* Конфигурационный файл для общих параметров модулей АВ-ТУК
 * Создан 19.10.2016
 */

#include "config.h"

Config_Main::Config_Main()
{
    ConfigMain[0] = {BCI_MTYPEB, u32_TYPE, sizeof(quint32), sizeof(BciMain_Def.MTypeB)/sizeof(quint32), &(BciMain_Def.MTypeB)};
    ConfigMain[1] = {BCI_MTYPEM, u32_TYPE, sizeof(quint32), sizeof(BciMain_Def.MTypeM)/sizeof(quint32), &(BciMain_Def.MTypeM)};
    ConfigMain[0] = {BCI_CTYPE, u16_TYPE, sizeof(quint16), sizeof(BciMain_Def.Ctype)/sizeof(quint16), &BciMain_Def.Ctype};
    ConfigMain[1] = {BCI_ABS_104, u32_TYPE, sizeof(quint32), sizeof(BciMain_Def.Abs_104)/sizeof(quint32), &BciMain_Def.Abs_104};
    ConfigMain[2] = {BCI_CYCLE_104, u32_TYPE, sizeof(quint32), sizeof(BciMain_Def.Cycle_104)/sizeof(quint32), &BciMain_Def.Cycle_104};
    ConfigMain[3] = {BCI_T1_104, u32_TYPE, sizeof(quint32), sizeof(BciMain_Def.T1_104)/sizeof(quint32), &BciMain_Def.T1_104};
    ConfigMain[4] = {BCI_T2_104, u32_TYPE, sizeof(quint32), sizeof(BciMain_Def.T2_104)/sizeof(quint32), &BciMain_Def.T2_104};
    ConfigMain[5] = {BCI_T3_104, u32_TYPE, sizeof(quint32), sizeof(BciMain_Def.T3_104)/sizeof(quint32), &BciMain_Def.T3_104};
    ConfigMain[6] = {BCI_K_104, u32_TYPE, sizeof(quint32), sizeof(BciMain_Def.k_104)/sizeof(quint32), &BciMain_Def.k_104};
    ConfigMain[7] = {BCI_W_104, u32_TYPE, sizeof(quint32), sizeof(BciMain_Def.w_104)/sizeof(quint32), &BciMain_Def.w_104};
    BciMain_Def = {2, 205, 5, 15, 10, 20, 12, 8};
}

