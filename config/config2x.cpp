#include "config2x.h"
#include "../publicclass.h"

bool config2x::SetAInSize(Bci_AIn &Bcii, int size)
{
    if (size > SIZEMAX)
        return false;
    Bcii.in_max.resize(size);
    Bcii.in_min.resize(size);
    Bcii.in_type.resize(size);
    Bcii.in_vmax.resize(size);
    Bcii.in_vmin.resize(size);
    Bcii.setmax.resize(size);
    Bcii.setmaxmax.resize(size);
    Bcii.setmin.resize(size);
    Bcii.setminmin.resize(size);
    Bcii.osc.resize(size);
}

bool config2x::SetAOutSize(Bci_AOut &Bcio, int size)
{
    if (size > SIZEMAX)
        return false;
    Bcio.out_max.resize(size);
    Bcio.out_min.resize(size);
    Bcio.out_type.resize(size);
    Bcio.out_vmax.resize(size);
    Bcio.out_vmin.resize(size);
}

bool config2x::Setup(int MType)
{
    if (MType < MT)
    SetAInSize(Bci_block.inblk, ModTypeMap().value(MType).AInSize);
    SetAOutSize(Bci_block.outblk, ModTypeMap().value(MType).AOutSize);
}

bool config2x::SetConfig()
{
    int StartInIndex = ModTypeMap().value(MType).AInStart;
    int StartOutIndex = ModTypeMap().value(MType).AOutStart;
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
    Config[10] = {StartInIndex, sizeof(Bci_block.inblk.in_type), Bci_block.inblk.in_type.data()};
    Config[11] = {StartInIndex+1, sizeof(Bci_block.inblk.in_min), Bci_block.inblk.in_min.data()};
    Config[12] = {StartInIndex+2, sizeof(Bci_block.inblk.in_max), Bci_block.inblk.in_max.data()};
    Config[13] = {StartInIndex+3, sizeof(Bci_block.inblk.in_vmin), Bci_block.inblk.in_vmin.data()};
    Config[14] = {StartInIndex+4, sizeof(Bci_block.inblk.in_vmax), Bci_block.inblk.in_vmax.data()};
    Config[15] = {StartInIndex+5, sizeof(Bci_block.inblk.setminmin), Bci_block.inblk.setminmin.data()};
    Config[16] = {StartInIndex+6, sizeof(Bci_block.inblk.setmin), Bci_block.inblk.setmin.data()};
    Config[17] = {StartInIndex+7, sizeof(Bci_block.inblk.setmax), Bci_block.inblk.setmax.data()};
    Config[18] = {StartInIndex+8, sizeof(Bci_block.inblk.setmaxmax), Bci_block.inblk.setmaxmax.data()};
    Config[19] = {StartInIndex+9, sizeof(Bci_block.inblk.osc), Bci_block.inblk.osc.data()};
    Config[20] = {StartInIndex+10, sizeof(Bci_block.inblk.oscdly), &Bci_block.inblk.oscdly};
    Config[21] = {StartInIndex+11, sizeof(Bci_block.inblk.oscthr), &Bci_block.inblk.oscthr};
    Config[22] = {StartInIndex+12, sizeof(Bci_block.inblk.filterthr), &Bci_block.inblk.filterthr};
    Config[23] = {StartInIndex+13, sizeof(Bci_block.inblk.filtern), &Bci_block.inblk.filtern};
    Config[24] = {StartInIndex+14, sizeof(Bci_block.inblk.hysteresis), &Bci_block.inblk.hysteresis};
    Config[25] = {0xFFFFFFFF, 0, NULL};
}
