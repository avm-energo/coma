#include "configkiv.h"

#include "../gen/s2.h"

ConfigKIV::ConfigKIV()
{
    //    m_S2S2Config = S2Config;
    m_mainConfig = new Config;
    m_KxxConfig = new ConfigKxx;
    // параметры входных сигналов
    quint32 StartInIndex = ID8084_START;
    //    quint32 StartComIndex = ID8084_COM;
    Config::removeFotter();

    if (StartInIndex != 0)
    {
        S2Config->append({ 1003, sizeof(Bci_block.NFiltr), &Bci_block.NFiltr });
        S2Config->append({ 1006, sizeof(Bci_block.Unom), &Bci_block.Unom });
        S2Config->append({ StartInIndex, sizeof(Bci_block.Umin), &Bci_block.Umin });
        S2Config->append({ StartInIndex + 1, sizeof(Bci_block.Imin), &Bci_block.Imin });
        S2Config->append({ StartInIndex + 3, sizeof(Bci_block.C_init), &Bci_block.C_init });
        S2Config->append({ StartInIndex + 4, sizeof(Bci_block.Tg_init), &Bci_block.Tg_init });
        S2Config->append({ StartInIndex + 5, sizeof(Bci_block.corTg), &Bci_block.corTg });
        S2Config->append({ StartInIndex + 6, sizeof(Bci_block.dC_pred), &Bci_block.dC_pred });
        S2Config->append({ StartInIndex + 7, sizeof(Bci_block.dC_alarm), &Bci_block.dC_alarm });
        S2Config->append({ StartInIndex + 8, sizeof(Bci_block.dTg_pred), &Bci_block.dTg_pred });
        S2Config->append({ StartInIndex + 9, sizeof(Bci_block.dTg_alarm), &Bci_block.dTg_alarm });
        S2Config->append({ StartInIndex + 10, sizeof(Bci_block.dIunb_pred), &Bci_block.dIunb_pred });
        S2Config->append({ StartInIndex + 11, sizeof(Bci_block.dIunb_alarm), &Bci_block.dIunb_alarm });
        S2Config->append({ StartInIndex + 12, sizeof(Bci_block.GdC), &Bci_block.GdC });
        S2Config->append({ StartInIndex + 13, sizeof(Bci_block.GdTg), &Bci_block.GdTg });
        S2Config->append({ StartInIndex + 14, sizeof(Bci_block.GdIunb), &Bci_block.GdIunb });
        S2Config->append({ StartInIndex + 17, sizeof(Bci_block.Iunb_init), &Bci_block.Iunb_init });
        S2Config->append({ StartInIndex + 18, sizeof(Bci_block.Phy_unb_init), &Bci_block.Phy_unb_init });
        S2Config->append({ StartInIndex + 19, sizeof(Bci_block.T_Data_Rec), &Bci_block.T_Data_Rec });
        S2Config->append({ StartInIndex + 20, sizeof(Bci_block.LowU), &Bci_block.LowU });
        S2Config->append({ StartInIndex + 21, sizeof(Bci_block.IsU), &Bci_block.IsU });
        S2Config->append({ StartInIndex + 22, sizeof(Bci_block.IsIunb), &Bci_block.IsIunb });
        S2Config->append({ StartInIndex + 23, sizeof(Bci_block.Tevent_pred), &Bci_block.Tevent_pred });
        S2Config->append({ StartInIndex + 24, sizeof(Bci_block.Tevent_alarm), &Bci_block.Tevent_alarm });
        S2Config->append({ StartInIndex + 27, sizeof(Bci_block.Tg_pasp), &Bci_block.Tg_pasp });
        S2Config->append({ StartInIndex + 28, sizeof(Bci_block.C_pasp), &Bci_block.C_pasp });
        S2Config->append({ 1050, sizeof(Bci_block.Unom_1), &Bci_block.Unom_1 });
    }
    S2Config->append({ 0xFFFFFFFF, 0, nullptr });
    //    DataBlock::BlockStruct dbs {
    //        1,
    //        "Конфигурация",
    //        DataBlock::DataBlockTypes::BciBlock,
    //        m_S2Config,
    //    };
    //    m_dblock = new DataBlock(dbs);
    //    struct BlockStruct
    //    {
    //        int blocknum;                        // number of the block to send corresponding command
    //        QString caption;                     // block name to set it to the GroupBox GUI
    //        DataBlock::DataBlockTypes blocktype; // type of the block to choose proper command
    //        void *block;    // pointer to the block, for S2Config blocks it's a pointer to S2ConfigDataType
    //        void *defblock; // pointer to the block with default values
    //        int blocksize;  // size of the block to make a mem copy
    //    };
}

void ConfigKIV::setDefConf()
{
    m_mainConfig->SetDefConf();
    m_KxxConfig->SetDefConf();
    Bci_block.Unom = 220;
    Bci_block.Unom_1 = 100;
    Bci_block.Umin = 0.5;
    Bci_block.Imin = 0.5;
    Bci_block.dC_pred = 0.5;
    Bci_block.dC_alarm = 1.0;
    Bci_block.dTg_pred = 1;
    Bci_block.dTg_alarm = 1.5;
    Bci_block.dIunb_pred = 1;
    Bci_block.dIunb_alarm = 1.5;
    Bci_block.GdC = 0.1f;
    Bci_block.GdTg = 0.2f;
    Bci_block.GdIunb = 0.2f;
    Bci_block.Iunb_init = 0;
    Bci_block.Phy_unb_init = 0;
    Bci_block.NFiltr = 100;
    Bci_block.T_Data_Rec = 60;
    Bci_block.LowU = 80;
    Bci_block.IsU = 1;
    Bci_block.IsIunb = 1;
    Bci_block.Tevent_pred = 0;
    Bci_block.Tevent_alarm = 0;

    for (int i = 0; i < 3; i++)
    {
        Bci_block.C_init[i] = 2200;
        Bci_block.Tg_init[i] = 0;
        Bci_block.corTg[i] = 0;
        Bci_block.Tg_pasp[i] = 0.3f;
        Bci_block.C_pasp[i] = 1500;
    }
}

// Error::Msg ConfigKIV::getAndPushConfig()
//{
//    if (Commands::GetFileWithRestore(CM_CONFIGFILE, m_S2Config) == Error::Msg::NoError)
//    {
//        Bci newBciBlock;
//        memcpy(&newBciBlock, &Bci_block, sizeof(Bci));
//        m_BciStack.push(newBciBlock);
//    }
//    else
//        return Error::Msg::GeneralError;
//    return Error::Msg::NoError;
//}

// ConfigKIV::Bci ConfigKIV::popConfig()
//{
//    if (m_BciStack.isEmpty())
//    {
//        Bci newBciBlock;
//        setDefConf(newBciBlock);
//        return newBciBlock;
//    }
//    return m_BciStack.pop();
//}

// Error::Msg ConfigKIV::popAndWriteConfig()
//{
//    Bci tempBciBlock;
//    memcpy(&tempBciBlock, &Bci_block, sizeof(Bci)); // push current Bci
//    Bci_block = popConfig();
//    Error::Msg err = Commands::WriteFile(CM_CONFIGFILE, m_S2Config);
//    memcpy(&Bci_block, &tempBciBlock, sizeof(Bci)); // pop current Bci
//    return err;
//}

// S2DataTypes::S2ConfigType *ConfigKIV::S2Config()
//{
//    return m_S2Config;
//}

// void ConfigKIV::setS2Config(S2DataTypes::S2ConfigType *s2config)
//{
//    m_S2Config = s2config;
//}

Config *ConfigKIV::MainConfig()
{
    return m_mainConfig;
}

ConfigKxx *ConfigKIV::KxxConfig()
{
    return m_KxxConfig;
}
