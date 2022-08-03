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
        { "Название", "nameInput", false },   //
        { "База: ", "bTypeInput", false },    //
        { "Мезонин: ", "mTypeInput", false }, //
        { "Версия", "verInput", false }       //
    };
    mTitle += "модуля";

    // https://github.com/KDE/clazy/blob/1.11/docs/checks/README-range-loop-detach.md
    for (const auto &itemSettings : qAsConst(dlgSettings))
    {
        auto labelText = std::get<0>(itemSettings);
        auto itemName = std::get<1>(itemSettings);
        auto labelItem = WDFunc::NewLBL2(this, labelText, itemName + "Label");
        auto inputItem = WDFunc::NewLE2(this, itemName);
        QObject::connect(
            inputItem, &QLineEdit::textEdited, this, qOverload<const QString &>(&ModuleDialog::dataChanged));
        if (std::get<2>(itemSettings) && inputItem != nullptr)
        {
            inputItem->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
        }

        auto itemLayout = new QHBoxLayout;
        itemLayout->addWidget(labelItem);
        itemLayout->addWidget(inputItem);
        mainLayout->addLayout(itemLayout);
        dlgItems.append(inputItem);
    }
}
