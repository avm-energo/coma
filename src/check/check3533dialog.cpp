#include "check3533dialog.h"

#include "../gen/datamanager.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <bitset>

constexpr uint16_t INPUT_QUANITY = 32;

constexpr int offset = 1;
constexpr int columns = 6;

Check3533Dialog::Check3533Dialog(const CheckItem &item, const categoryMap &categories, QWidget *parent)
    : CheckDialog(item, categories, parent)
{
    Timer->setInterval(ANMEASINT);
    connect(&DataManager::GetInstance(), &DataManager::bitStringReceived, this, &Check3533Dialog::updateBitStringData);
}

void Check3533Dialog::setupUI()
{
    m_BdUIList = { { "Основные", Bd1W() } };
    m_BdUIList.first().widget->setUpdatesEnabled();

    AbstractCheckDialog::setupUI();
}

void Check3533Dialog::updatePixmap(bool isset, int position)
{
    auto pixmap = WDFunc::NewCircle((isset) ? m_alarmColor : m_normalColor, circleRadius);
    auto status = WDFunc::SetLBLImage(this, QString::number(position), &pixmap);
    if (!status)
        qCritical() << Error::DescError;
}

void Check3533Dialog::updateBitStringData(const DataTypes::BitStringStruct &bs)
{
    if (bs.sigAdr != 5001)
        return;
    std::bitset<sizeof(bs.sigVal) * 8> values = bs.sigVal;
    for (auto i = 0; i != values.size(); ++i)
    {
        updatePixmap(values.test(i), i + offset);
    }
}

UWidget *Check3533Dialog::Bd1W()
{
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QGroupBox *gb = new QGroupBox("Общие");

    glyout->addWidget(WDFunc::NewLBL2(this, "Температура микроконтроллера, °С"), 0, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "101", ValuesFormat, "Температура микроконтроллера, °С"), 1, 0, 1, 1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);
    gb = new QGroupBox("Состояние входов");
    glyout = new QGridLayout;

    for (int i = 0; i < INPUT_QUANITY; ++i)
    {

        QHBoxLayout *hlyout = new QHBoxLayout;
        auto labelText = new QLabel(QString::number(i + offset) + ":", this);
        hlyout->addWidget(labelText);

        auto pixmap = WDFunc::NewCircle(m_normalColor, circleRadius);
        auto label = new QLabel(this);
        label->setObjectName(QString::number(i + offset));
        label->setPixmap(pixmap);

        hlyout->addWidget(label);
        glyout->addLayout(hlyout, i / columns, i % columns);
    }
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    w->setLayout(lyout);
    w->setFloatBdQuery({ { 101, 2 } });
    w->setBsBdQuery({ { 5001, 1 } });

    return w;
}
