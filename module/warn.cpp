#include "warn.h"

#include "../gen/datamanager.h"
#include "../widgets/wd_func.h"

#include <QVBoxLayout>
#include <QtDebug>

Warn::Warn(QWidget *parent) : UWidget(parent)
{
    m_actualWarnFlags = 0x0000;
    connect(&DataManager::GetInstance(), &DataManager::singlePointReceived, this, &Warn::updateSpData);
}

int Warn::realWarnSize()
{
    return m_realWarnSize;
}

/*void Warn::Update(std::bitset<32> &states)
{
    for (int i = 0; i < m_realWarnSize; ++i)
    {
        updatePixmaps(states[i], i);
    }
} */

void Warn::showEvent(QShowEvent *e)
{
    if (isVisible())
        move(QCursor::pos());
    e->accept();
}

void Warn::updatePixmap(bool isset, int position)
{
    auto pixmap = WDFunc::NewCircle((isset) ? Qt::yellow : Qt::green, CIRCLE_RADIUS);
    WDFunc::SetLBLImage(this, (QString::number(position)), &pixmap);
    m_actualWarnFlags[position] = isset;
    emit updateWarn(m_actualWarnFlags != 0x0000);
    //    emit updateWarn(position, isset);
    //    if (isset)
    //    {
    //        auto pixmap = WDFunc::NewCircle(Qt::yellow, CIRCLE_RADIUS);
    //        WDFunc::SetLBLImage(this, (QString::number(position)), &pixmap);
    //    }
    //    else
    //    {
    //        auto pixmap = WDFunc::NewCircle(Qt::green, CIRCLE_RADIUS);
    //        WDFunc::SetLBLImage(this, (QString::number(position)), &pixmap);
    //    }
}

void Warn::updateSpData(DataTypes::SinglePointWithTimeStruct &sp)
{
    bool warnFlag = false; // warning flag
    quint32 minAddress = m_startWarnAddress;
    quint32 maxAddress = m_startWarnAddress + 31; // only 32 bits
    if ((sp.sigAdr >= minAddress) && (sp.sigAdr <= maxAddress))
    {
        int index = sp.sigAdr - minAddress;
        quint8 sigval = sp.sigVal;
        if (!(sigval & 0x80))
        {
            if (m_warnFlags[index])
            {
                warnFlag = true;
                updatePixmap(sigval & 0x00000001, index);
            }
        }
    }
}

void Warn::setupUI(const QStringList &events)
{
    m_realWarnSize = events.size();
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");

    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    for (int i = 0; i < m_realWarnSize; ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "transparent", QString::number(i)));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    w->setLayout(vlayout);

    lyout->addWidget(w);
    lyout->addWidget(WDFunc::NewPB(this, "", "Ok", this, SLOT(hide())), 0);
    setLayout(lyout);
}
