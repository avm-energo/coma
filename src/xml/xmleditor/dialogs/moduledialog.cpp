#include "moduledialog.h"

#include "../../../widgets/wd_func.h"

#include <QGuiApplication>
#include <QScreen>

ModuleDialog::ModuleDialog(Behaviour behav, QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI(behav);
    //    this->exec();
}

void ModuleDialog::SetupUI(Behaviour &behav)
{
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    SetupSizePos(330, 200);
    QList<std::pair<QString, QString>> dlgSettings = {
        { "База: ", "bTypeInput" },    //
        { "Мезонин: ", "mTypeInput" }, //
        { "Версия", "verInput" },      //
        { "Название", "nameInput" }    //
    };

    QList<QWidget *> dlgItems;
    for (const auto &itemSettings : dlgSettings)
    {
        auto uiItem = WDFunc::NewLBLAndLE(this, itemSettings.first, itemSettings.second, true);
        mainLayout->addWidget(uiItem);
        dlgItems.append(uiItem);
    }

    // [[maybe_unused]] auto bTypeItem = WDFunc::NewLBLAndLE(this, "База: ", "bTypeInput", true);
    // [[maybe_unused]] auto mTypeItem = WDFunc::NewLBLAndLE(this, "Мезонин: ", "mTypeInput", true);
    // [[maybe_unused]] auto verItem = WDFunc::NewLBLAndLE(this, "Версия", "verInput", true);
    // [[maybe_unused]] auto nameItem = WDFunc::NewLBLAndLE(this, "Название", "nameInput", true);
    // mainLayout->addWidget(bTypeItem);
    // mainLayout->addWidget(mTypeItem);
    // mainLayout->addWidget(verItem);
    // mainLayout->addWidget(nameItem);

    if (behav == Behaviour::Create)
    {
        setWindowTitle("Создание конфигурации модуля");
    }
    else
    {
        setWindowTitle("Редактирование конфигурации модуля");
        ;
        ;
    }
    setLayout(mainLayout);
}

void ModuleDialog::SetupSizePos(int width, int height)
{
    auto center = QGuiApplication::primaryScreen()->geometry().center();
    setGeometry(center.x() - width / 2, center.y() - height / 2, width, height);
    setFixedSize(this->size());
}
