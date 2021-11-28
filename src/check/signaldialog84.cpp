#include "signaldialog84.h"

#include <QHBoxLayout>

SignalDialog84::SignalDialog84(QWidget *parent) : SignalDialog(parent), m_bdaIn(new BdaIn)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_bdaIn->widget());
    setLayout(layout);
}

void SignalDialog84::reqUpdate()
{
    if (!updatesEnabled())
        return;
    m_bdaIn->readAndUpdate();
}
