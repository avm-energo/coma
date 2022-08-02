#include "moduledialog.h"

#include "../../../widgets/wd_func.h"

#include <QGuiApplication>
#include <QScreen>

ModuleDialog::ModuleDialog(QWidget *parent) : QDialog(parent), isChanged(false)
{
    dlgSettings = {
        { "База: ", "bTypeInput", false },    //
        { "Мезонин: ", "mTypeInput", false }, //
        { "Версия", "verInput", false },      //
        { "Название", "nameInput", false }    //
    };
    setAttribute(Qt::WA_DeleteOnClose);
}

void ModuleDialog::SetData(const int row)
{
    mRow = row;
    dlgItems.reserve(dlgSettings.count());
}

void ModuleDialog::SetupUI(int width, int height, const QString &title)
{
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    SetupSizePos(width, height);
    // https://github.com/KDE/clazy/blob/1.11/docs/checks/README-range-loop-detach.md
    for (const auto &itemSettings : qAsConst(dlgSettings))
    {
        auto idName = std::get<1>(itemSettings);
        auto uiItem = WDFunc::NewLBLAndLE(this, std::get<0>(itemSettings), idName, true);
        if (std::get<2>(itemSettings))
        {
            auto input = uiItem->findChild<QLineEdit *>(idName);
            if (input != nullptr)
                input->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
        }
        mainLayout->addWidget(uiItem);
        dlgItems.append(uiItem);
    }
    auto saveBtn = WDFunc::NewPB(this, "saveBtn", "Сохранить", this, &ModuleDialog::SaveData);
    mainLayout->addWidget(saveBtn);

    if (mRow == createId)
    {
        setWindowTitle("Создание " + title);
        isChanged = true;
    }
    else
    {
        setWindowTitle("Редактирование " + title);
        emit ModelDataRequest(mRow);
    }
    setLayout(mainLayout);
    this->exec();
}

void ModuleDialog::SetupSizePos(int &width, int &height)
{
    auto center = QGuiApplication::primaryScreen()->geometry().center();
    setGeometry(center.x() - width / 2, center.y() - height / 2, width, height);
    setFixedSize(size());
}

// TODO: Must be virtual
void ModuleDialog::ModelDataResponse(const QStringList &response)
{
    for (auto i = 0; i < dlgSettings.count(); i++)
    {
        auto idName = std::get<1>(dlgSettings[i]);
        auto input = dlgItems[i]->findChild<QLineEdit *>(idName);
        if (input != nullptr)
        {
            QObject::connect(
                input, &QLineEdit::textEdited, this, qOverload<const QString &>(&ModuleDialog::DataChanged));
            input->setText(response[i]);
        }
    }
}

void ModuleDialog::DataChanged()
{
    if (!isChanged)
    {
        isChanged = true;
    }
}

void ModuleDialog::DataChanged(const QString &str)
{
    Q_UNUSED(str);
    DataChanged();
    ;
}

void ModuleDialog::SaveData()
{
    if (isChanged)
    {
        QStringList saved;
        for (auto i = 0; i < dlgItems.count(); i++)
        {
            auto idName = std::get<1>(dlgSettings[i]);
            auto input = dlgItems[i]->findChild<QLineEdit *>(idName);
            if (input != nullptr)
                saved.append(input->text());
        }
        if (mRow == createId)
            emit CreateData(saved, &mRow);
        else
            emit UpdateData(saved, mRow);
    }
}
