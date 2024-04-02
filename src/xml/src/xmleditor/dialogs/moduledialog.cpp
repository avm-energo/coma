#include "xml/xmleditor/dialogs/moduledialog.h"

#include <QGuiApplication>
#include <QScreen>
#include <variant>
#include <widgets/wd_func.h>

constexpr auto bTypeIndex = 1;
constexpr auto versionIndex = 3;

ModuleDialog::ModuleDialog(QWidget *parent) : XmlDialog(parent)
{
}

void ModuleDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(380, 220);
    m_dlgSettings = {
        { "Название", "nameInput" },   //
        { "База: ", "bTypeInput" },    //
        { "Мезонин: ", "mTypeInput" }, //
        { "Версия", "verInput" }       //
    };
    m_title += "модуля";
    for (const auto &itemSettings : qAsConst(m_dlgSettings))
    {
        auto labelText = itemSettings.first;
        auto itemName = itemSettings.second;
        auto labelItem = WDFunc::NewLBL2(this, labelText, itemName + "Label");
        auto inputItem = WDFunc::NewLE2(this, itemName);
        QObject::connect(
            inputItem, &QLineEdit::textEdited, this, qOverload<const QString &>(&ModuleDialog::dataChanged));

        auto itemLayout = new QHBoxLayout;
        itemLayout->addWidget(labelItem);
        itemLayout->addWidget(inputItem);
        mainLayout->addLayout(itemLayout);
        m_dlgItems.append(inputItem);
    }
}

QStringList ModuleDialog::collectData()
{
    auto retVal = XmlDialog::collectData();
    // Не сохраняем версию для мезонинных плат
    if (retVal[bTypeIndex] == "00")
        retVal[versionIndex] = "No version";
    return retVal;
}

void ModuleDialog::loadModelData(const QStringList &response)
{
    XmlDialog::loadModelData(response);
    // Деактивируем поле версии для мезонинных плат
    if (response[bTypeIndex] == "00")
    {
        auto versionInput = std::get<QLineEdit *>(m_dlgItems[versionIndex]);
        versionInput->setEnabled(false);
    }
}
