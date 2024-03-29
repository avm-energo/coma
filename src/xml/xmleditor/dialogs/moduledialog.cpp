#include "moduledialog.h"

#include "../../../widgets/wd_func.h"

#include <QGuiApplication>
#include <QScreen>
#include <variant>

constexpr auto bTypeIndex = 1;
constexpr auto versionIndex = 3;

ModuleDialog::ModuleDialog(QWidget *parent) : XmlDialog(parent)
{
}

void ModuleDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(380, 220);
    dlgSettings = {
        { "Название", "nameInput" },   //
        { "База: ", "bTypeInput" },    //
        { "Мезонин: ", "mTypeInput" }, //
        { "Версия", "verInput" }       //
    };
    mTitle += "модуля";
    for (const auto &itemSettings : qAsConst(dlgSettings))
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
        dlgItems.append(inputItem);
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

void ModuleDialog::modelDataResponse(const QStringList &response)
{
    XmlDialog::modelDataResponse(response);
    // Деактивируем поле версии для мезонинных плат
    if (response[bTypeIndex] == "00")
    {
        auto versionInput = std::get<QLineEdit *>(dlgItems[versionIndex]);
        versionInput->setEnabled(false);
    }
}
