#include "configktf.h"

#include "config.h"

ConfigKTF::ConfigKTF(QVector<S2::DataRec> *config)
{

    for (int i = 0; i < config->size(); ++i)
    {
        if (config->at(i).id == 0xFFFFFFFF)
        {
            config->remove(i);
            --i;
        }
    }

    config->append({ BCI_NFILTR, sizeof(Bci_block.NFiltr), &Bci_block.NFiltr });
    config->append({ BCI_NHARMFILT, sizeof(Bci_block.NHarmFilt), &Bci_block.NHarmFilt });
    config->append({ BCI_DDOCS, sizeof(Bci_block.DDosc), &Bci_block.DDosc });
    config->append({ BCI_UNOM1, sizeof(Bci_block.Unom1), &Bci_block.Unom1 });
    config->append({ BCI_DUOSC, sizeof(Bci_block.DUosc), &Bci_block.DUosc });
    config->append({ BCI_DIOSC, sizeof(Bci_block.DIosc), &Bci_block.DIosc });
    config->append({ BCI_DUIMIN, sizeof(Bci_block.DUImin), &Bci_block.DUImin });
    config->append({ BCI_IMIN, sizeof(Bci_block.Imin), &Bci_block.Imin });
    config->append({ BCI_T_DATA_REC, sizeof(Bci_block.T_Data_Rec), &Bci_block.T_Data_Rec });
    config->append({ BCI_U2NOM, sizeof(Bci_block.U2nom), &Bci_block.U2nom });
    config->append({ BCI_ITT1NOM, sizeof(Bci_block.ITT1nom), &Bci_block.ITT1nom });
    config->append({ BCI_ITT2NOM, sizeof(Bci_block.ITT2nom), &Bci_block.ITT2nom });
    config->append({ BCI_COOL_TYPE, sizeof(Bci_block.Cool_type), &Bci_block.Cool_type });
    config->append({ BCI_W_MAT, sizeof(Bci_block.W_mat), &Bci_block.W_mat });
    config->append({ BCI_TNNTDOP, sizeof(Bci_block.TNNTdop), &Bci_block.TNNTdop });
    config->append({ BCI_TNNTPRED, sizeof(Bci_block.TNNTpred), &Bci_block.TNNTpred });
    config->append({ BCI_TAMB_NOM, sizeof(Bci_block.Tamb_nom), &Bci_block.Tamb_nom });
    config->append({ BCI_DTNNT_NOM, sizeof(Bci_block.dTNNTnom), &Bci_block.dTNNTnom });
    config->append({ BCI_KDOB, sizeof(Bci_block.Kdob), &Bci_block.Kdob });
    config->append({ BCI_TAUWNOM, sizeof(Bci_block.TauWnom), &Bci_block.TauWnom });
    config->append({ BCI_UMAX, sizeof(Bci_block.Umax), &Bci_block.Umax });
    config->append({ BCI_IMAX, sizeof(Bci_block.Imax), &Bci_block.Imax });
    config->append({ BCI_IWNOM, sizeof(Bci_block.Iwnom), &Bci_block.Iwnom });
    config->append({ BCI_OSCPOINTS, sizeof(Bci_block.OscPoints), &Bci_block.OscPoints });
    config->append({ BCI_GTNNT, sizeof(Bci_block.GTnnt), &Bci_block.GTnnt });
    config->append({ BCI_GOVC, sizeof(Bci_block.GOvc), &Bci_block.GOvc });
    config->append({ BCI_TDATNUM, sizeof(Bci_block.TdatNum), &Bci_block.TdatNum });

    config->append({ 0xFFFFFFFF, 0, nullptr });
}

void ConfigKTF::SetDefConf()
{

    Bci_block.NFiltr = DEF_NFILTR;
    Bci_block.NHarmFilt = DEF_NHARMFILT;
    Bci_block.DDosc = DEF_DDOCS;
    Bci_block.Unom1 = DEF_UNOM1KTF;
    Bci_block.DUosc = DEF_DUOSC;
    Bci_block.DIosc = DEF_DIOSC;
    Bci_block.DUImin = DEF_DUIMIN;
    Bci_block.Imin = static_cast<float>(DEF_IMIN);
    Bci_block.T_Data_Rec = DEF_T_DATA_RECKTF;
    Bci_block.U2nom = DEF_U2NOM;
    Bci_block.ITT1nom = DEF_ITT1NOM;
    Bci_block.ITT2nom = DEF_ITT2NOM;
    Bci_block.Cool_type = DEF_COOL_TYPE;
    Bci_block.W_mat = DEF_W_MAT;
    Bci_block.TNNTdop = DEF_TNNTDOP;
    Bci_block.TNNTpred = DEF_TNNTPRED;
    Bci_block.Tamb_nom = DEF_TAMB_NOM;
    Bci_block.dTNNTnom = DEF_DTNNT_NOM;
    Bci_block.Kdob = static_cast<float>(DEF_KDOB);
    Bci_block.TauWnom = DEF_TAUWNOM;
    Bci_block.Umax = DEF_UMAX;
    Bci_block.Imax = DEF_IMAX;
    Bci_block.Iwnom = DEF_IWNOM;
    Bci_block.OscPoints = DEF_OSCPOINTS;
    Bci_block.GTnnt = DEF_GTNNT;
    Bci_block.GOvc = DEF_GOVC;
    Bci_block.TdatNum = DEF_TDATNUM;
}
