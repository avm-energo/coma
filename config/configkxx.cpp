#include "configkxx.h"

ConfigKxx::ConfigKxx(S2ConfigType *config)
{
    //    for (int i = 0; i < config->size(); ++i)
    //    {
    //        if (config->at(i).id == 0xFFFFFFFF)
    //        {
    //            config->remove(i);
    //            --i;
    //        }
    //    }
    /// TODO Check this
    config->erase(std::remove_if(config->begin(), config->end(), [](S2::DataRec i) { return i.id == 0xFFFFFFFF; }),
        config->end());
    /// TODO end
    ///
    config->append({ BCI_RTERM, sizeof(TempConf.RTerm), &TempConf.RTerm });
    config->append({ BCI_W100, sizeof(TempConf.W100), &TempConf.W100 });

    config->append({ BCI_TRELE_PRED, sizeof(StrTrele.Trele_pred), &StrTrele.Trele_pred });
    config->append({ BCI_TRELE_ALARM, sizeof(StrTrele.Trele_alarm), &StrTrele.Trele_alarm });

    config->append({ BCI_MBMASTER, sizeof(StrModBus.MBMaster), &StrModBus.MBMaster });
    config->append({ BCI_MBMAB1, sizeof(StrModBus.MBMab1), &StrModBus.MBMab1 });
    config->append({ BCI_MBMAB2, sizeof(StrModBus.MBMab2), &StrModBus.MBMab2 });
    config->append({ BCI_MBMAB3, sizeof(StrModBus.MBMab3), &StrModBus.MBMab3 });
    config->append({ BCI_MBMAB4, sizeof(StrModBus.MBMab4), &StrModBus.MBMab4 });

    config->append({ BCI_IP, sizeof(Com_param.IP), &Com_param.IP });
    config->append({ BCI_MASK, sizeof(Com_param.Mask), &Com_param.Mask });
    config->append({ BCI_GW, sizeof(Com_param.GateWay), &Com_param.GateWay });
    config->append({ BCI_PORT, sizeof(Com_param.Port), &Com_param.Port });
    config->append({ BCI_SNTP, sizeof(Com_param.SNTP), &Com_param.SNTP });
    config->append({ BCI_BAUD, sizeof(Com_param.Baud), &Com_param.Baud });
    config->append({ BCI_PARITY, sizeof(Com_param.Parity), &Com_param.Parity });
    config->append({ BCI_STOPBIT, sizeof(Com_param.Stopbit), &Com_param.Stopbit });
    config->append({ BCI_ADRMB, sizeof(Com_param.adrMB), &Com_param.adrMB });

    config->append({ 0xFFFFFFFF, 0, nullptr });
}

void ConfigKxx::SetDefConf()
{
    TempConf.RTerm = DEF_RTERM;
    TempConf.W100 = static_cast<float>(DEF_W100);

    StrTrele.Trele_pred = DEF_TRELE_PRED;
    StrTrele.Trele_alarm = DEF_TRELE_ALARM;

    StrModBus.MBMaster = DEF_MBMASTER;

    for (int i = 0; i < 8; i++)
    {

        StrModBus.MBMab1[i] = 0;
        StrModBus.MBMab2[i] = 0;
        StrModBus.MBMab3[i] = 0;
        StrModBus.MBMab4[i] = 0;
    }

    Com_param.IP[0] = 172;
    Com_param.IP[1] = 16;
    Com_param.IP[2] = 29;
    Com_param.IP[3] = 12;

    Com_param.Mask[0] = 255;
    Com_param.Mask[1] = 255;
    Com_param.Mask[2] = 252;
    Com_param.Mask[3] = 0;

    Com_param.GateWay[0] = 172;
    Com_param.GateWay[1] = 16;
    Com_param.GateWay[2] = 29;
    Com_param.GateWay[3] = 1;

    Com_param.Port[0] = 2404;
    Com_param.Port[1] = 2405;
    Com_param.Port[2] = 502;
    Com_param.Port[3] = 502;

    Com_param.SNTP[0] = 172;
    Com_param.SNTP[1] = 16;
    Com_param.SNTP[2] = 31;
    Com_param.SNTP[3] = 220;

    Com_param.Baud = DEF_BAUD;
    Com_param.Parity = DEF_PARITY;
    Com_param.Stopbit = DEF_STOPBIT;
    Com_param.adrMB = DEF_ADRMB;
}
