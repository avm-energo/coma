#ifndef CORDIALOG_H
#define CORDIALOG_H

#include <QDialog>
#include <QByteArray>
#include "../gen/mainwindow.h"
#include "../widgets/etablemodel.h"

class CorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CorDialog(QWidget *parent = nullptr);
    ~CorDialog();

    int corDIndex;

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
    void FillBd(QWidget *parent, QString Name, QString Value);

signals:
   void sendCom45(quint32*);
   void sendCom50(quint16* adr, float *data);


public slots:
    void GetCorBd(int index);
    void GetCorBdButton();
    void WriteCorBd();
    void WriteCor();
    void WriteCorCapacity();
    void WriteCorTg();
    void MessageOk();
    void SetCor();
    void ResetCor();
    void UpdateFlCorData(Parse104::FlSignals104 *Signal);

private slots:

};

#endif // CORDIALOG_H
