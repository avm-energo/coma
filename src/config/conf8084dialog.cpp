#include "conf8084dialog.h"

#include <QGroupBox>
#include <QTabWidget>
#include <QVBoxLayout>
Conf8084Dialog::Conf8084Dialog(ConfigKIV *ckiv, QWidget *parent) : ConfKIVDialog(ckiv, parent)
{
}

void Conf8084Dialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");

    ConfTW->addTab(analogWidget(), "Аналоговые");

    ConfTW->addTab(thresholdsWidget(), "Уставки");

    ConfTW->addTab(remainsWidget(), "Остальное");

    ConfTW->addTab(connectionWidget(), "Связь");

    lyout->addWidget(ConfTW);
    lyout->addWidget(ConfButtons());
    setLayout(lyout);
}

void Conf8084Dialog::Fill()
{
    CKIV->MainConfig()->Fill();
    FillKiv();
}

void Conf8084Dialog::FillBack()
{
    CKIV->MainConfig()->FillBack();
    FillBackKiv();
}

QWidget *Conf8084Dialog::connectionWidget()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;

    QGridLayout *gridlyout = new QGridLayout;

    gb->setTitle("Настройки протокола МЭК-60870-5-104");

    gridlyout->addWidget(CKIV->MainConfig()->MainWidget(this), 0, 0, 1, 1);

    gb->setLayout(gridlyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Настройка времени");

    vlyout->addWidget(CKIV->MainConfig()->TimeWidget(this));

    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    w->setLayout(lyout);
    return w;
}
