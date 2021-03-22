#ifndef CONFIG
#define CONFIG

#include "../gen/datatypes.h"
#include "../module/modules.h"
#include "baseconfig.h"

#include <QMap>

class Config : BaseConfig
{
    enum CtypeBits
    {
        SNTP_Addr1 = 0,
        PPS = 1,
        SNTP_Addr2 = 2,
        SNTP_Default = 3
    };

public:
    explicit Config();
    ~Config() {};

    void SetDefConf() override;
    QWidget *MainWidget(QWidget *parent);
    QWidget *TimeWidget(QWidget *parent);
    void Fill() override;
    void FillBack() const override;

    static void removeFotter();

private:
    QObject *ParentMainbl, *ParentCtype;
};

#endif // CONFIG
