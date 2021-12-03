#include "check3133dialog.h"

#include "../gen/datamanager.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <bitset>
#include <boost/dynamic_bitset.hpp>

constexpr int offset = 1;
constexpr int columns = 6;

Check3133Dialog::Check3133Dialog(const CheckItem &item, const categoryMap &categories, QWidget *parent)
    : CheckDialog(item, categories, parent)
{
    Timer->setInterval(ANMEASINT);
    connect(&DataManager::GetInstance(), &DataManager::bitStringReceived, this, &Check3133Dialog::updateBitStringData);
}

void Check3133Dialog::setupUI()
{
    m_BdUIList = { { "Основные", Bd1W() } };
    m_BdUIList.first().widget->setUpdatesEnabled();

    AbstractCheckDialog::setupUI();
}

void Check3133Dialog::updatePixmap(bool isset, int position)
{
    auto pixmap = WDFunc::NewCircle((isset) ? m_alarmColor : m_normalColor, circleRadius);
    auto status = WDFunc::SetLBLImage(this, QString::number(position), &pixmap);
    if (!status)
        qCritical() << Error::DescError;
}

void Check3133Dialog::updateBitStringData(const DataTypes::BitStringStruct &bs)
{
    auto result
        = std::find_if(m_item.signlsVec.cbegin(), m_item.signlsVec.cend(), [&bs](const check::detail::Signals &sig) {
              return (sig.start_addr == bs.sigAdr)
                  && (sig.type == ctti::unnamed_type_id<DataTypes::BitStringStruct>().hash());
          });
    if (result == m_item.signlsVec.end())
        return;

    auto values = boost::dynamic_bitset(inputQuantity(), bs.sigVal);
    const auto valuesSize = values.size();
    const auto inputSize = inputQuantity();
    assert(valuesSize == inputSize);
    for (auto i = 0; i != values.size(); ++i)
    {
        updatePixmap(values.test(i), i + offset);
    }
}

UWidget *CheckMezz3133Dialog::Bd1W()
{
    UWidget *w = new UWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox("Состояние входов");
    QGridLayout *glyout = new QGridLayout;

    for (int i = 0; i != inputQuantity(); ++i)
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

    addSignals(*m_item.signlsVec.front().groups.cbegin(), w);

    return w;
}

UWidget *CheckBase3133Dialog::Bd1W()
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

    for (int i = 0; i != inputQuantity(); ++i)
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

    addSignals(*m_item.signlsVec.front().groups.cbegin(), w);

    return w;
}
