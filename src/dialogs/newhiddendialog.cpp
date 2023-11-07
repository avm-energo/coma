#include "newhiddendialog.h"

#include "../widgets/wd_func.h"

#include <QBoxLayout>
#include <QGroupBox>
#include <QPaintEvent>
#include <QPainter>
#include <QTabWidget>
#include <QtSvg/QSvgRenderer>

namespace crypto
{
static constexpr char hash[] = "fb001dfcffd1c899f3297871406242f097aecf1a5342ccf3ebcd116146188e4b";
static constexpr char name[] = "hiddenHash";
}

NewHiddenDialog::NewHiddenDialog(const ModuleTypes::HiddenSettings &settings, QWidget *parent)
    : UDialog(crypto::hash, crypto::name, parent), m_settings(settings)
{
    if (m_settings.empty())
        generateDefaultSettings();
    setupUI();
}

void NewHiddenDialog::generateDefaultSettings()
{
    using namespace ModuleTypes;
    m_settings = {
        HiddenTab {
            "Базовая плата", ":/images/BMn.svg",
            {
                HiddenWidget { "basetype", "Тип платы", 40010, BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget {
                    "baseserial", "Серийный номер платы", 40011, BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget { "baseversion", "Версия платы", 40012, BinaryType::uint32, ViewType::Version, true },
                HiddenWidget {
                    "moduleserial", "Серийный номер модуля", 40013, BinaryType::uint32, ViewType::LineEdit, true } //
            }                                                                                                      //
        },
        HiddenTab {
            "Мезонинная плата", ":/images/BnM.svg",
            {
                HiddenWidget { "mezztype", "Тип платы", 40014, BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget {
                    "mezzserial", "Серийный номер платы", 40015, BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget { "mezzversion", "Версия платы", 40016, BinaryType::uint32, ViewType::Version, true },
                HiddenWidget { "reserved", "Резерв", 40017, BinaryType::uint32, ViewType::LineEdit, false } //
            }                                                                                               //
        }
    };
}

void NewHiddenDialog::setupUI()
{
    auto layout = new QVBoxLayout;
    auto tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet("background-color: transparent;");

    for (auto &&tabSettings : m_settings)
    {
        auto tab = new QWidget(this);
        auto tabLayout = new QVBoxLayout;
        // DO SOMETHING
        tab->setLayout(tabLayout);
        tabWidget->addTab(tab, tabSettings.title);
    }

    m_currentBackground = m_settings[0].background;
    connect(tabWidget, &QTabWidget::currentChanged, this, //
        [tabWidget, this](int newIndex) {
            if (newIndex >= 0 && newIndex < m_settings.size())
                m_currentBackground = m_settings[newIndex].background;
            update();
        });
    layout->addWidget(tabWidget);
    setLayout(layout);
}

void NewHiddenDialog::setupGroupBox(const ModuleTypes::HiddenTab &hiddenTab)
{
    auto tabGroupBox = new QGroupBox(hiddenTab.title, this);
    auto gbLayout = new QVBoxLayout;
    for (auto &&widget : hiddenTab.widgets)
    {
        if (widget.visibility)
        {
            if (widget.view == ModuleTypes::ViewType::Version)
            {
                auto hLayout = new QHBoxLayout;
                WDFunc::AddLabelAndLineeditH(hLayout, widget.title + ':', widget.name + 'm', false);
                WDFunc::AddLabelAndLineeditH(hLayout, ".", widget.name + 'l', false);
                WDFunc::AddLabelAndLineeditH(hLayout, ".", widget.name + 's', false);
                gbLayout->addLayout(hLayout);
            }
            else
            {
                gbLayout->addWidget(WDFunc::NewLBLAndLE(this, widget.title + ':', widget.name, false));
            }
        }
    }
    tabGroupBox->setLayout(gbLayout);
}

void NewHiddenDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QSvgRenderer svg(m_currentBackground);
    svg.render(&painter);
    painter.end();
    e->accept();
}
