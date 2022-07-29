#include "moduledialog.h"

#include "../../../widgets/wd_func.h"

#include <QGuiApplication>
#include <QScreen>

ModuleDialog::ModuleDialog(const QStringList &inData, QWidget *parent) : QDialog(parent)
{
    dlgSettings = {
        { "База: ", "bTypeInput", false },    //
        { "Мезонин: ", "mTypeInput", false }, //
        { "Версия", "verInput", false },      //
        { "Название", "nameInput", false }    //
    };
    dlgItems.reserve(dlgSettings.count());
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI(inData);
    //    this->exec();
}

void ModuleDialog::SetupUI(const QStringList &inData)
{
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    SetupSizePos(330, 200);
    // https://github.com/KDE/clazy/blob/1.11/docs/checks/README-range-loop-detach.md
    for (const auto &itemSettings : qAsConst(dlgSettings))
    {
        auto idName = std::get<1>(itemSettings);
        auto uiItem = WDFunc::NewLBLAndLE(this, std::get<0>(itemSettings), idName, true);
        if (std::get<2>(itemSettings))
        {
            auto input = uiItem->findChild<QLineEdit *>(idName);
            input->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
        }
        mainLayout->addWidget(uiItem);
        dlgItems.append(uiItem);
    }

    if (inData.isEmpty())
    {
        setWindowTitle("Создание конфигурации модуля");
    }
    else
    {
        Q_ASSERT(inData.count() >= 4);
        setWindowTitle("Редактирование конфигурации модуля");
    }
    setLayout(mainLayout);
}

void ModuleDialog::SetupSizePos(int width, int height)
{
    auto center = QGuiApplication::primaryScreen()->geometry().center();
    setGeometry(center.x() - width / 2, center.y() - height / 2, width, height);
    setFixedSize(this->size());
}
