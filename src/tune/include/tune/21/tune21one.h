#pragma once

#include <datablocks/21/bac.h>
#include <datablocks/21/bda.h>
#include <tune/abstracttunedialog.h>

class Tune21One final : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune21One(int tuneStep, Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    Bac21 *m_bac;
    Bda21 *m_bda;
    int m_curTuneStep;
    int m_BacWidgetIndex, m_BacWidgetIndex2, m_BdainWidgetIndex, m_Bd0WidgetIndex;

    void setTuneFunctions() override;

    Error::Msg SendBac();

    QWidget *BdWidget();

private slots:
    void CalcTuneCoefs();
};
