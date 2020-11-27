#ifndef CONFIG
#define CONFIG

#include "../gen/datatypes.h"
#include "../module/modules.h"

#include <QMap>

class Config
{
public:
    explicit Config(S2DataTypes::S2ConfigType *config);

    Bci::BciMain MainBlk;

    void setConfig(S2DataTypes::S2ConfigType *config);
    void SetDefConf();
    QWidget *MainWidget(QWidget *parent);
    QWidget *TimeWidget(QWidget *parent);
    void Fill();
    void FillBack();

    static void removeFotter(S2DataTypes::S2ConfigType *config);

private:
    QObject *ParentMainbl, *ParentCtype;

    QMap<Bci::BciMainEnum, quint32 *> regs {
        { Bci::BciMainEnum::MTypeB, &MainBlk.MTypeB },       //
        { Bci::BciMainEnum::MTypeM, &MainBlk.MTypeM },       //
        { Bci::BciMainEnum::Ctype, &MainBlk.Ctype },         //
        { Bci::BciMainEnum::Abs_104, &MainBlk.Abs_104 },     //
        { Bci::BciMainEnum::Cycle_104, &MainBlk.Cycle_104 }, //
        { Bci::BciMainEnum::T1_104, &MainBlk.T1_104 },       //
        { Bci::BciMainEnum::T2_104, &MainBlk.T2_104 },       //
        { Bci::BciMainEnum::T3_104, &MainBlk.T3_104 },       //
        { Bci::BciMainEnum::K_104, &MainBlk.K_104 },         //
        { Bci::BciMainEnum::W_104, &MainBlk.W_104 },         //
        { Bci::BciMainEnum::MTypeB, &MainBlk.MTypeB }        //
    };
};

#endif // CONFIG
