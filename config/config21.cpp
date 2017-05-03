#include "config21.h"

config_21::config_21()
{
    SetAInSize(Bci_block.inblk, MOD_NUMINPUTS21);

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
    for (int i = 0; i < MOD_NUMINPUTS21; i++)
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
