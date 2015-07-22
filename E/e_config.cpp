#include "e_config.h"

e_config::e_config()
{
    Config[0] = {EBCI_MTYPE, u32_TYPE, sizeof(quint32), sizeof(Bci_block.MType)/sizeof(quint32), &(Bci_block.MType)};
    Config[1] = {EBCI_MTYPE1, u32_TYPE, sizeof(quint32), sizeof(Bci_block.MType1)/sizeof(quint32), &(Bci_block.MType1)};
    Config[2] = {EBCI_EQTYPE, u32_TYPE, sizeof(quint32), sizeof(Bci_block.eq_type)/sizeof(quint32), &(Bci_block.eq_type)};
    Config[3] = {EBCI_NPOINTS, u32_TYPE, sizeof(quint32), sizeof(Bci_block.npoints)/sizeof(quint32), &(Bci_block.npoints)};
    Config[4] = {EBCI_NFILTR, u32_TYPE, sizeof(quint32), sizeof(Bci_block.nfiltr)/sizeof(quint32), &Bci_block.nfiltr};
    Config[5] = {EBCI_NHFILTR, u32_TYPE, sizeof(quint32), sizeof(Bci_block.nhfiltr)/sizeof(quint32), &Bci_block.nhfiltr};
    Config[6] = {EBCI_DDOSC, u32_TYPE, sizeof(quint32), sizeof(Bci_block.ddosc)/sizeof(quint32), &Bci_block.ddosc};
    Config[7] = {EBCI_UNOM1, float_TYPE, sizeof(float), sizeof(Bci_block.unom1)/sizeof(float), &Bci_block.unom1};
    Config[8] = {EBCI_UNOM2, float_TYPE, sizeof(float), sizeof(Bci_block.unom2)/sizeof(float), &Bci_block.unom2};
    Config[9] = {EBCI_INOM1, float_TYPE, sizeof(float), sizeof(Bci_block.inom1)/sizeof(float), &Bci_block.inom1};
    Config[10] = {EBCI_INOM2, float_TYPE, sizeof(float), sizeof(Bci_block.inom2)/sizeof(float), &Bci_block.inom2};
    Config[11] = {EBCI_DUOSC, float_TYPE, sizeof(float), sizeof(Bci_block.duosc)/sizeof(float), &Bci_block.duosc};
    Config[12] = {EBCI_DIOSC, float_TYPE, sizeof(float), sizeof(Bci_block.diosc)/sizeof(float), &Bci_block.diosc};
    Config[13] = {EBCI_DUIMIN, float_TYPE, sizeof(float), sizeof(Bci_block.duimin)/sizeof(float), &Bci_block.duimin};
    Config[14] = {EBCI_CTYPE, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Ctype)/sizeof(quint32), &Bci_block.Ctype};
    Config[15] = {EBCI_ABS_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Abs_104)/sizeof(quint32), &Bci_block.Abs_104};
    Config[16] = {EBCI_CYCLE_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Cycle_104)/sizeof(quint32), &Bci_block.Cycle_104};
    Config[17] = {EBCI_T1_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T1_104)/sizeof(quint32), &Bci_block.T1_104};
    Config[18] = {EBCI_T2_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T2_104)/sizeof(quint32), &Bci_block.T2_104};
    Config[19] = {EBCI_T3_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T3_104)/sizeof(quint32), &Bci_block.T3_104};
    Config[20] = {EBCI_K_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.k_104)/sizeof(quint32), &Bci_block.k_104};
    Config[21] = {EBCI_W_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.w_104)/sizeof(quint32), &Bci_block.w_104};
    Config[22] = {0xFFFF, 0, 0, 0, NULL};
    Bci_defblock.MType = 4;
    Bci_defblock.MType1 = 0x000001;
    Bci_defblock.Abs_104 = 205;
    Bci_defblock.Ctype = 1;
    Bci_defblock.Cycle_104 = 5;
    Bci_defblock.T1_104 = 15;
    Bci_defblock.T2_104 = 10;
    Bci_defblock.T3_104 = 20;
    Bci_defblock.k_104 = 12;
    Bci_defblock.w_104 = 8;
    Bci_defblock.eq_type = 1;
    Bci_defblock.npoints = 128;
    Bci_defblock.nfiltr = 100;
    Bci_defblock.nhfiltr = 3;
    Bci_defblock.ddosc = 1;
    Bci_defblock.unom1 = 500;
    Bci_defblock.unom2 = 220;
    Bci_defblock.duosc = 2.0;
    Bci_defblock.diosc = 2.0;
    Bci_defblock.duimin = 0.5;
    for (int i = 0; i < 3; i++)
    {
        Bci_defblock.inom1[i] = 600;
        Bci_defblock.inom1[i+3] = 1000;
        Bci_defblock.inom2[i] = Bci_defblock.inom2[i+3] = 5;
    }
}

e_config::~e_config()
{

}

