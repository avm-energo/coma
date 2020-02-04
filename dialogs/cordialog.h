#ifndef CORDIALOG_H
#define CORDIALOG_H

#include <QDialog>
#include <QByteArray>

#include "../widgets/etablemodel.h"

class CorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CorDialog(QWidget *parent = nullptr);
    ~CorDialog();

private:


    struct CorData
    {
        float C_init[3]; //Начальные значени емкостей вводов
        float Tg_init[3]; //Начальные значения tgδ вводов
        float corTg[3];	// коррекция  tg δ вводов
        float Iunb_init;		// Начальное действ. значение тока небаланса
        float Phy_unb_init;	// Начальное значение угла тока небаланса
    };

    CorData *CorBlock;

    void FillCor();
    void FillBackCor();
    float ToFloat(QString text);
    void SetupUI();

signals:
   void sendCom45(quint32*);
   void sendCom50(quint16* adr, float *data);


public slots:
    void GetCorBd();
    void WriteCorBd();
    void WriteCorNotBalance();
    void WriteCorCapacity();
    void WriteCorTg();
    void MessageOk();
    void SetCor();
    void ResetCor();

private slots:
#if PROGSIZE != PROGSIZE_EMUL

#endif
};

#endif // CORDIALOG_H
