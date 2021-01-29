#ifndef CHECKHARMONICKDV_H
#define CHECKHARMONICKDV_H

#include <QWidget>

class CheckHarmonicKDV
{
public:
    CheckHarmonicKDV();

    struct Bd_5_7
    {
        float UI_Harm[3][61];
    };

    Bd_5_7 Bd_block5, Bd_block7;

    void FillBd5(QWidget *parent);
    void FillBd7(QWidget *parent);

    void FillBd(QWidget *parent, QString Name, QString Value);

    QWidget *BdUWidget(QWidget *parent, int value,int min, int max);
    QWidget *BdIWidget(QWidget *parent, int value,int min, int max);


private:
    QString ValuesFormat, WidgetFormat;
};

#endif // CHECKHARMONICKDV_H
