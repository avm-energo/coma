#include "configkiv.h"

#include "../usb/commands.h"

ConfigKIV::ConfigKIV(S2ConfigType *config)
{
    m_S2Config = config;
    // параметры входных сигналов
    quint32 StartInIndex = ID8084_START;
    //    quint32 StartComIndex = ID8084_COM;
    for (int i = 0; i < config->size(); ++i)
    {
        if (config->at(i).id == 0xFFFFFFFF)
        {
            config->remove(i);
            --i;
        }
    }

    if (StartInIndex != 0)
    {

        config->append({ 1003, sizeof(Bci_block.NFiltr), &Bci_block.NFiltr });
        config->append({ 1006, sizeof(Bci_block.Unom), &Bci_block.Unom });
        config->append({ StartInIndex, sizeof(Bci_block.Umin), &Bci_block.Umin });
        config->append({ StartInIndex + 1, sizeof(Bci_block.Imin), &Bci_block.Imin });
        config->append({ StartInIndex + 3, sizeof(Bci_block.C_init), &Bci_block.C_init });
        config->append({ StartInIndex + 4, sizeof(Bci_block.Tg_init), &Bci_block.Tg_init });
        config->append({ StartInIndex + 5, sizeof(Bci_block.corTg), &Bci_block.corTg });
        config->append({ StartInIndex + 6, sizeof(Bci_block.dC_pred), &Bci_block.dC_pred });
        config->append({ StartInIndex + 7, sizeof(Bci_block.dC_alarm), &Bci_block.dC_alarm });
        config->append({ StartInIndex + 8, sizeof(Bci_block.dTg_pred), &Bci_block.dTg_pred });
        config->append({ StartInIndex + 9, sizeof(Bci_block.dTg_alarm), &Bci_block.dTg_alarm });
        config->append({ StartInIndex + 10, sizeof(Bci_block.dIunb_pred), &Bci_block.dIunb_pred });
        config->append({ StartInIndex + 11, sizeof(Bci_block.dIunb_alarm), &Bci_block.dIunb_alarm });
        config->append({ StartInIndex + 12, sizeof(Bci_block.GdC), &Bci_block.GdC });
        config->append({ StartInIndex + 13, sizeof(Bci_block.GdTg), &Bci_block.GdTg });
        config->append({ StartInIndex + 14, sizeof(Bci_block.GdIunb), &Bci_block.GdIunb });
        config->append({ StartInIndex + 17, sizeof(Bci_block.Iunb_init), &Bci_block.Iunb_init });
        config->append({ StartInIndex + 18, sizeof(Bci_block.Phy_unb_init), &Bci_block.Phy_unb_init });
        config->append({ StartInIndex + 19, sizeof(Bci_block.T_Data_Rec), &Bci_block.T_Data_Rec });
        config->append({ StartInIndex + 20, sizeof(Bci_block.LowU), &Bci_block.LowU });
        config->append({ StartInIndex + 21, sizeof(Bci_block.IsU), &Bci_block.IsU });
        config->append({ StartInIndex + 22, sizeof(Bci_block.IsIunb), &Bci_block.IsIunb });
        config->append({ StartInIndex + 23, sizeof(Bci_block.Tevent_pred), &Bci_block.Tevent_pred });
        config->append({ StartInIndex + 24, sizeof(Bci_block.Tevent_alarm), &Bci_block.Tevent_alarm });
        config->append({ StartInIndex + 27, sizeof(Bci_block.Tg_pasp), &Bci_block.Tg_pasp });
        config->append({ StartInIndex + 28, sizeof(Bci_block.C_pasp), &Bci_block.C_pasp });
        config->append({ 1050, sizeof(Bci_block.Unom_1), &Bci_block.Unom_1 });
    }
    config->append({ 0xFFFFFFFF, 0, nullptr });
    DataBlock::BlockStruct dbs {
        1,
        "Конфигурация",
        DataBlock::DataBlockTypes::BciBlock,
        m_S2Config,
    };
    m_dblock = new DataBlock(dbs);
    struct BlockStruct
    {
        int blocknum;                        // number of the block to send corresponding command
        QString caption;                     // block name to set it to the GroupBox GUI
        DataBlock::DataBlockTypes blocktype; // type of the block to choose proper command
        void *block;    // pointer to the block, for S2Config blocks it's a pointer to S2ConfigDataType
        void *defblock; // pointer to the block with default values
        int blocksize;  // size of the block to make a mem copy
    };
}

void ConfigKIV::setDefConf()
{
    setDefConf(Bci_block);
}

void ConfigKIV::setDefConf(Bci &bci)
{
    bci.Unom = 220;
    bci.Unom_1 = 100;
    bci.Umin = 0.5;
    bci.Imin = 0.5;
    bci.dC_pred = 0.5;
    bci.dC_alarm = 1.0;
    bci.dTg_pred = 1;
    bci.dTg_alarm = 1.5;
    bci.dIunb_pred = 1;
    bci.dIunb_alarm = 1.5;
    bci.GdC = 0.1f;
    bci.GdTg = 0.2f;
    bci.GdIunb = 0.2f;
    bci.Iunb_init = 0;
    bci.Phy_unb_init = 0;
    bci.NFiltr = 100;
    bci.T_Data_Rec = 60;
    bci.LowU = 80;
    bci.IsU = 1;
    bci.IsIunb = 1;
    bci.Tevent_pred = 0;
    bci.Tevent_alarm = 0;

    for (int i = 0; i < 3; i++)
    {
        bci.C_init[i] = 2200;
        bci.Tg_init[i] = 0;
        bci.corTg[i] = 0;
        bci.Tg_pasp[i] = 0.3f;
        bci.C_pasp[i] = 1500;
    }
}

Error::Msg ConfigKIV::getAndPushConfig()
{
    if (Commands::GetFileWithRestore(CM_CONFIGFILE, m_S2Config) == Error::Msg::NoError)
    {
        Bci newBciBlock;
        memcpy(&newBciBlock, &Bci_block, sizeof(Bci));
        m_BciStack.push(newBciBlock);
    }
    else
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

ConfigKIV::Bci ConfigKIV::popConfig()
{
    if (m_BciStack.isEmpty())
    {
        Bci newBciBlock;
        setDefConf(newBciBlock);
        return newBciBlock;
    }
    return m_BciStack.pop();
}

Error::Msg ConfigKIV::popAndWriteConfig()
{
    Bci tempBciBlock;
    memcpy(&tempBciBlock, &Bci_block, sizeof(Bci)); // push current Bci
    Bci_block = popConfig();
    Error::Msg err = Commands::WriteFile(CM_CONFIGFILE, m_S2Config);
    memcpy(&Bci_block, &tempBciBlock, sizeof(Bci)); // pop current Bci
    return err;
}

S2ConfigType *ConfigKIV::S2Config()
{
    return m_S2Config;
}

void ConfigKIV::setS2Config(S2ConfigType *s2config)
{
    m_S2Config = s2config;
}
