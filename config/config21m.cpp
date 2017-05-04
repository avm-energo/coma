#include "config21m.h"

Config21m::Config21m()
{
    Config = new Config2x(MTM_21);

    Config->Bci_defblock.mainblk.MTypeB = MTB_21;
    Config->Bci_defblock.mainblk.MTypeM = MTM_00;
    Config->Bci_defblock.mainblk.Abs_104 = DEF_ABS_104;
    Config->Bci_defblock.mainblk.Ctype = DEF_CTYPE;
    Config->Bci_defblock.mainblk.Cycle_104 = DEF_CYCLE_104;
    Config->Bci_defblock.mainblk.k_104 = DEF_K_104;
    Config->Bci_defblock.mainblk.w_104 = DEF_W_104;
    Config->Bci_defblock.mainblk.T1_104 = DEF_T1_104;
    Config->Bci_defblock.mainblk.T2_104 = DEF_T2_104;
    Config->Bci_defblock.mainblk.T3_104 = DEF_T3_104;
    Config->Bci_defblock.oscdly = 0;
    Config->Bci_defblock.oscthr = 0.05f;
    Config->Bci_defblock.filtern = 10;
    Config->Bci_defblock.filterthr = 0.5;
    Config->Bci_defblock.hysteresis = 0.02f;
    for (int i = 0; i < MOD_NUMINPUTS21; i++)
    {
        Config->Bci_defblock.in_type[i] = INTYPEMA;
        Config->Bci_defblock.in_min[i] = 4;
        Config->Bci_defblock.in_max[i] = 20;
        Config->Bci_defblock.in_vmin[i] = 0;
        Config->Bci_defblock.in_vmax[i] = 1000;
        Config->Bci_defblock.setminmin[i] = 10;
        Config->Bci_defblock.setmin[i] = 50;
        Config->Bci_defblock.setmax[i] = 950;
        Config->Bci_defblock.setmaxmax[i] = 990;
        Config->Bci_defblock.osc[i] = 0;
    }
}
