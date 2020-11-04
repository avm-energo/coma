#ifndef CHECKHARMONICKTF_H
#define CHECKHARMONICKTF_H

#include <QWidget>

class CheckHarmonicKTF
{
public:
    CheckHarmonicKTF();

    struct Bd5
    {
        float UI_Harm[3][61];
    };

    struct Bd7
    {
        float UI_Harm[3][61];
    };

    Bd7 Bd_block7;
    Bd5 Bd_block5;

    void FillBd5(QWidget *parent);
    void FillBd7(QWidget *parent);

    void FillBd(QWidget *parent, QString Name, QString Value);

     void BdWidget(QWidget *parent,QString Name, QString Value);

     QWidget *BdUWidget(QWidget *parent, int value,int min, int max);
     QWidget *BdIWidget(QWidget *parent, int value,int min, int max);


private:
    QString ValuesFormat, WidgetFormat;
};

#endif // CHECKHARMONICKTF_H
