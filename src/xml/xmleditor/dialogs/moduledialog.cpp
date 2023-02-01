#include "moduledialog.h"

#include "../../../widgets/wd_func.h"

#include <QGuiApplication>
#include <QScreen>

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
