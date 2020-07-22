#include "configktf.h"
#include "configdefktf.h"
#include "config.h"

ConfigKTF::ConfigKTF(QVector<S2::DataRec> *config)
{

    // параметры входных сигналов
//    quint32 StartInIndex = ID8084_START;
    quint32 StartComIndex = ID_COM;
    for (int i=0; i<config->size(); ++i)
    {
        if (config->at(i).id == 0xFFFFFFFF)
        {
            config->remove(i);
            --i;
        }
    }



 //   if ((StartInIndex != 0))
//    {
        config->append({BCI_MTYPEB, sizeof(MainBlk.MTypeB_ID), &MainBlk.MTypeB_ID});
        config->append({BCI_MTYPEM, sizeof(MainBlk.MTypeE_ID), &MainBlk.MTypeE_ID});
        config->append({BCI_CTYPE, sizeof(MainBlk.Ctype), &MainBlk.Ctype});
        config->append({BCI_ABS_104, sizeof(MainBlk.Abs_104), &MainBlk.Abs_104});
        config->append({BCI_CYCLE_104, sizeof(MainBlk.Cycle_104), &MainBlk.Cycle_104});
        config->append({BCI_T1_104, sizeof(MainBlk.T1_104), &MainBlk.T1_104});
        config->append({BCI_T2_104, sizeof(MainBlk.T2_104), &MainBlk.T2_104});
        config->append({BCI_T3_104, sizeof(MainBlk.T3_104), &MainBlk.T3_104});
        config->append({BCI_K_104, sizeof(MainBlk.k_104), &MainBlk.k_104});
        config->append({BCI_W_104, sizeof(MainBlk.w_104), &MainBlk.w_104});

        config->append({BCI_NFILTR_ID, sizeof(Bci_block.NFiltr_ID), &Bci_block.NFiltr_ID});
        config->append({BCI_NHARMFILT_ID, sizeof(Bci_block.NHarmFilt_ID), &Bci_block.NHarmFilt_ID});
        config->append({BCI_DDOCS_ID, sizeof(Bci_block.DDosc_ID), &Bci_block.DDosc_ID});
        config->append({BCI_UNOM1, sizeof(Bci_block.Unom1), &Bci_block.Unom1});
        //config->append({StartInIndex+2, sizeof(Bci_block.Imax), &Bci_block.Imax});
        config->append({BCI_DUOSC, sizeof(Bci_block.DUosc), &Bci_block.DUosc});
        config->append({BCI_DIOSC_ID, sizeof(Bci_block.DIosc_ID), &Bci_block.DIosc_ID});
        config->append({BCI_DUIMIN_ID, sizeof(Bci_block.DUImin_ID), &Bci_block.DUImin_ID});
        config->append({BCI_IMIN, sizeof(Bci_block.Imin), &Bci_block.Imin});
        config->append({BCI_RTERM, sizeof(Bci_block.RTerm), &Bci_block.RTerm});
        config->append({BCI_W100, sizeof(Bci_block.W100), &Bci_block.W100});
        config->append({BCI_T_DATA_REC, sizeof(Bci_block.T_Data_Rec), &Bci_block.T_Data_Rec});
        config->append({BCI_TRELE_PRED, sizeof(Bci_block.Trele_pred), &Bci_block.Trele_pred});
        config->append({BCI_TRELE_ALARM, sizeof(Bci_block.Trele_alarm), &Bci_block.Trele_alarm});
        config->append({BCI_U2NOM, sizeof(Bci_block.U2nom), &Bci_block.U2nom});
        config->append({BCI_ITT1NOM, sizeof(Bci_block.ITT1nom), &Bci_block.ITT1nom});
        config->append({BCI_ITT2NOM, sizeof(Bci_block.ITT2nom), &Bci_block.ITT2nom});
        config->append({BCI_MBMASTER, sizeof(Bci_block.MBMaster), &Bci_block.MBMaster});
        config->append({BCI_MBMAB1, sizeof(Bci_block.MBMab1), &Bci_block.MBMab1});
        config->append({BCI_MBMAB2, sizeof(Bci_block.MBMab2), &Bci_block.MBMab2});
        config->append({BCI_MBMAB3, sizeof(Bci_block.MBMab3), &Bci_block.MBMab3});
        config->append({BCI_MBMAB4, sizeof(Bci_block.MBMab4), &Bci_block.MBMab4});
        config->append({BCI_COOL_TYPE, sizeof(Bci_block.Cool_type), &Bci_block.Cool_type});
        config->append({BCI_W_MAT, sizeof(Bci_block.W_mat), &Bci_block.W_mat});
        config->append({BCI_TNNTDOP, sizeof(Bci_block.TNNTdop), &Bci_block.TNNTdop});
        config->append({BCI_TNNTPRED, sizeof(Bci_block.TNNTpred), &Bci_block.TNNTpred});
        config->append({BCI_TAMB_NOM, sizeof(Bci_block.Tamb_nom), &Bci_block.Tamb_nom});
        config->append({BCI_DTNNT_NOM, sizeof(Bci_block.dTNNTnom), &Bci_block.dTNNTnom});
        config->append({BCI_KDOB, sizeof(Bci_block.Kdob), &Bci_block.Kdob});
        config->append({BCI_TAUWNOM, sizeof(Bci_block.TauWnom), &Bci_block.TauWnom});
        config->append({BCI_UMAXM, sizeof(Bci_block.Umaxm), &Bci_block.Umaxm});
        config->append({BCI_IMAXM, sizeof(Bci_block.Imaxm), &Bci_block.Imaxm});
        config->append({BCI_IWNOM, sizeof(Bci_block.Iwnom), &Bci_block.Iwnom});
        config->append({BCI_OSCPOINTS, sizeof(Bci_block.OscPoints), &Bci_block.OscPoints});
        config->append({BCI_GTNNT, sizeof(Bci_block.GTnnt), &Bci_block.GTnnt});
        config->append({BCI_GOVC, sizeof(Bci_block.GOvc), &Bci_block.GOvc});
        config->append({BCI_TDATNUM, sizeof(Bci_block.TdatNum), &Bci_block.TdatNum});

        //if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
        //{
          config->append({BCI_IP_ID, sizeof(Com_param.IP_ID), &Com_param.IP_ID});
          config->append({BCI_MASK_ID, sizeof(Com_param.Mask_ID), &Com_param.Mask_ID});
          config->append({BCI_GW_ID, sizeof(Com_param.GW_ID), &Com_param.GW_ID});
          config->append({BCI_PORT_ID, sizeof(Com_param.Port_ID), &Com_param.Port_ID});
          config->append({BCI_SNTP_ID, sizeof(Com_param.SNTP_ID), &Com_param.SNTP_ID});
          config->append({BCI_BAUD_ID, sizeof(Com_param.Baud_ID), &Com_param.Baud_ID});
          config->append({BCI_PARITY_ID, sizeof(Com_param.Parity_ID), &Com_param.Parity_ID});
          config->append({BCI_STOPBIT_ID, sizeof(Com_param.Stopbit_ID), &Com_param.Stopbit_ID});
          config->append({BCI_ADRMB_ID, sizeof(Com_param.adrMB_ID), &Com_param.adrMB_ID});
          config->append({BCI_ISNTP_ID, sizeof(Com_param.IsNTP_ID), &Com_param.IsNTP_ID});
          config->append({BCI_ISPPS_ID, sizeof(Com_param.IsPPS_ID), &Com_param.IsPPS_ID});
       // }
//    }
          config->append({0xFFFFFFFF, 0, nullptr});

}

void ConfigKTF::SetDefConf()
{
    MainBlk.Abs_104 = DEF_ABS_104;
    MainBlk.Ctype = DEF_CTYPE;
    MainBlk.Cycle_104 = DEF_CYCLE_104;
    MainBlk.k_104 = DEF_K_104;
    MainBlk.w_104 = DEF_W_104;
    MainBlk.T1_104 = DEF_T1_104;
    MainBlk.T2_104 = DEF_T2_104;
    MainBlk.T3_104 = DEF_T3_104;

    Bci_block.NFiltr_ID = DEF_NFILTR_ID;
    Bci_block.NHarmFilt_ID = DEF_NHARMFILT_ID;
    Bci_block.DDosc_ID = DEF_DDOCS_ID;
    Bci_block.Unom1 = DEF_UNOM1;
    Bci_block.DUosc = DEF_DUOSC;
    Bci_block.DIosc_ID = DEF_DIOSC_ID ;
    Bci_block.DUImin_ID = DEF_DIOSC_ID   ;
    Bci_block.Imin = DEF_IMIN;
    Bci_block.RTerm = DEF_RTERM;
    Bci_block.W100 = DEF_W100;
  //  Bci_block.T_Data_Rec = DEF_T_DATA_REC;
    Bci_block.Trele_pred = DEF_TRELE_PRED;
    Bci_block.Trele_alarm = DEF_TRELE_ALARM;
    Bci_block.U2nom = DEF_U2NOM ;
    Bci_block.ITT1nom = DEF_ITT1NOM ;
    Bci_block.ITT2nom = DEF_ITT2NOM  ;
    Bci_block.MBMaster = DEF_MBMASTER;
    Bci_block.Cool_type =DEF_COOL_TYPE;
    Bci_block.W_mat = DEF_W_MAT;
    Bci_block.TNNTdop = DEF_TNNTDOP;
    Bci_block.TNNTpred = DEF_TNNTPRED  ;
    Bci_block.Tamb_nom = DEF_TAMB_NOM;
    Bci_block.dTNNTnom = DEF_DTNNT_NOM;
    Bci_block.Kdob = DEF_KDOB;
    Bci_block.TauWnom = DEF_TAUWNOM;
    Bci_block.Umaxm = DEF_UMAXM;
    Bci_block.Imaxm = DEF_IMAXM;
    Bci_block.Iwnom = DEF_IWNOM;
    Bci_block.OscPoints = DEF_OSCPOINTS;
    Bci_block.GTnnt = DEF_GTNNT;
    Bci_block.GOvc = DEF_GOVC;
    Bci_block.TdatNum = DEF_TDATNUM;

    for (int i = 0; i < 4; i++)
    {

        Bci_block.MBMab1[i] = 0;
        Bci_block.MBMab2[i] = 0;
        Bci_block.MBMab3[i] = 0;
        Bci_block.MBMab4[i] = 0;

    }



    //if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    //{
      //  Bci_block.RTerm = 100;
      //  Bci_block.W100 = 1.385f;

        Com_param.IP_ID[0] = 172;
        Com_param.IP_ID[1] = 16;
        Com_param.IP_ID[2] = 29;
        Com_param.IP_ID[3] = 12;

        Com_param.Mask_ID[0] = 255;
        Com_param.Mask_ID[1] = 255;
        Com_param.Mask_ID[2] = 252;
        Com_param.Mask_ID[3] = 0;

        Com_param.GW_ID[0] = 172;
        Com_param.GW_ID[1] = 16;
        Com_param.GW_ID[2] = 29;
        Com_param.GW_ID[3] = 1;

        Com_param.Port_ID[0] = 2404;
        Com_param.Port_ID[1] = 2405;
        Com_param.Port_ID[2] = 502;
        Com_param.Port_ID[3] = 502;

        Com_param.SNTP_ID[0] = 172;
        Com_param.SNTP_ID[1] = 16;
        Com_param.SNTP_ID[2] = 31;
        Com_param.SNTP_ID[3] = 220;

        Com_param.Baud_ID = DEF_BAUD_ID;
        Com_param.Parity_ID = DEF_PARITY_ID;
        Com_param.Stopbit_ID = DEF_STOPBIT_ID;
        Com_param.adrMB_ID = DEF_STOPBIT_ID;
        Com_param.IsNTP_ID = DEF_ISNTP_ID;
       // Com_param.IsPPS_ID = DEF_ISPPS_ID;

    //}
}



