#include "config21.h"

config_21::config_21()
{
    Config[0] = {BCI_MTYPEB, sizeof(Bci_block.mainblk.MTypeB), &(Bci_block.mainblk.MTypeB)};
    Config[1] = {BCI_MTYPEM, sizeof(Bci_block.mainblk.MTypeM), &(Bci_block.mainblk.MTypeM)};
    Config[2] = {BCI_CTYPE, sizeof(Bci_block.mainblk.Ctype), &Bci_block.mainblk.Ctype};
    Config[3] = {BCI_ABS_104, sizeof(Bci_block.mainblk.Abs_104), &Bci_block.mainblk.Abs_104};
    Config[4] = {BCI_CYCLE_104, sizeof(Bci_block.mainblk.Cycle_104), &Bci_block.mainblk.Cycle_104};
    Config[5] = {BCI_T1_104, sizeof(Bci_block.mainblk.T1_104), &Bci_block.mainblk.T1_104};
    Config[6] = {BCI_T2_104, sizeof(Bci_block.mainblk.T2_104), &Bci_block.mainblk.T2_104};
    Config[7] = {BCI_T3_104, sizeof(Bci_block.mainblk.T3_104), &Bci_block.mainblk.T3_104};
    Config[8] = {BCI_K_104, sizeof(Bci_block.mainblk.k_104), &Bci_block.mainblk.k_104};
    Config[9] = {BCI_W_104, sizeof(Bci_block.mainblk.w_104), &Bci_block.mainblk.w_104};
    Config[10] = {ABCI_INTYPE8, sizeof(Bci_block.in_type), &(Bci_block.in_type)};
    Config[11] = {ABCI_INMIN8, sizeof(Bci_block.in_min), &(Bci_block.in_min)};
    Config[12] = {ABCI_INMAX8, sizeof(Bci_block.in_max), &Bci_block.in_max};
    Config[13] = {ABCI_INVMIN8, sizeof(Bci_block.in_vmin), &Bci_block.in_vmin};
    Config[14] = {ABCI_INVMAX8, sizeof(Bci_block.in_vmax), &Bci_block.in_vmax};
    Config[15] = {ABCI_SETMINMIN8, sizeof(Bci_block.setminmin), &Bci_block.setminmin};
    Config[16] = {ABCI_SETMIN8, sizeof(Bci_block.setmin), &Bci_block.setmin};
    Config[17] = {ABCI_SETMAX8, sizeof(Bci_block.setmax), &Bci_block.setmax};
    Config[18] = {ABCI_SETMAXMAX8, sizeof(Bci_block.setmaxmax), &Bci_block.setmaxmax};
    Config[19] = {ABCI_OSC8, sizeof(Bci_block.osc), &Bci_block.osc};
    Config[20] = {ABCI_OSCDLY, sizeof(Bci_block.oscdly), &Bci_block.oscdly};
    Config[21] = {ABCI_OSCTHR, sizeof(Bci_block.oscthr), &Bci_block.oscthr};
    Config[22] = {ABCI_FILTERTHR, sizeof(Bci_block.filterthr), &Bci_block.filterthr};
    Config[23] = {ABCI_FILTERN, sizeof(Bci_block.filtern), &Bci_block.filtern};
    Config[24] = {ABCI_HYSTERESIS, sizeof(Bci_block.hysteresis), &Bci_block.hysteresis};
    Config[25] = {0xFFFFFFFF, 0, NULL};

    Bci_defblock.mainblk.MTypeB = MTB_21;
    Bci_defblock.mainblk.MTypeM = MTM_00;
    Bci_defblock.mainblk.Abs_104 = DEF_ABS_104;
    Bci_defblock.mainblk.Ctype = DEF_CTYPE;
    Bci_defblock.mainblk.Cycle_104 = DEF_CYCLE_104;
    Bci_defblock.mainblk.k_104 = DEF_K_104;
    Bci_defblock.mainblk.w_104 = DEF_W_104;
    Bci_defblock.mainblk.T1_104 = DEF_T1_104;
    Bci_defblock.mainblk.T2_104 = DEF_T2_104;
    Bci_defblock.mainblk.T3_104 = DEF_T3_104;
    Bci_defblock.oscdly = 0;
    Bci_defblock.oscthr = 0.05f;
    Bci_defblock.filtern = 10;
    Bci_defblock.filterthr = 0.5;
    Bci_defblock.hysteresis = 0.02f;
    for (int i = 0; i < 8; i++)
    {
        Bci_defblock.in_type[i] = INTYPEMA;
        Bci_defblock.in_min[i] = 4;
        Bci_defblock.in_max[i] = 20;
        Bci_defblock.in_vmin[i] = 0;
        Bci_defblock.in_vmax[i] = 1000;
        Bci_defblock.setminmin[i] = 10;
        Bci_defblock.setmin[i] = 50;
        Bci_defblock.setmax[i] = 950;
        Bci_defblock.setmaxmax[i] = 990;
        Bci_defblock.osc[i] = 0;
    }
}
