#ifndef CONFIG
#define CONFIG

#include "../gen/datatypes.h"
#include "../module/modules.h"

#include <QMap>

class Config
{
public:
    explicit Config();

    Bci::BciMain MainBlk {};

    //  void setConfig();
    void SetDefConf();
    QWidget *MainWidget(QWidget *parent);
    QWidget *TimeWidget(QWidget *parent);
    void Fill();
    void FillBack();

    static void removeFotter();

private:
    QObject *ParentMainbl, *ParentCtype;
};

#endif // CONFIG
