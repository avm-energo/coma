#include "moduledialog.h"

#include "../../../widgets/wd_func.h"

#include <QGuiApplication>
#include <QScreen>

ModuleDialog::ModuleDialog(QWidget *parent) : QDialog(parent), isChanged(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

void ModuleDialog::SetData(const int row)
{
    dlgSettings = {
        { "База: ", "bTypeInput", false },    //
        { "Мезонин: ", "mTypeInput", false }, //
        { "Версия", "verInput", false },      //
        { "Название", "nameInput", false }    //
    };
    mRow = row;
    dlgItems.reserve(dlgSettings.count());
    SetupUI(320, 220, " модуля");
}

void ModuleDialog::SetupUI(int width, int height, const QString &title)
{
    // Создание слоёв окна
    auto mainLayout = new QVBoxLayout(this);
    SetupSizePos(width, height);
    // https://github.com/KDE/clazy/blob/1.11/docs/checks/README-range-loop-detach.md
    for (const auto &itemSettings : qAsConst(dlgSettings))
    {
        auto labelText = std::get<0>(itemSettings);
        auto itemName = std::get<1>(itemSettings);
        auto labelItem = WDFunc::NewLBL2(this, labelText, itemName + "Label");
        auto inputItem = WDFunc::NewLE2(this, itemName);
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
    auto saveBtn = WDFunc::NewPB(this, "saveBtn", "Сохранить", this, &ModuleDialog::SaveData);
    mainLayout->addWidget(saveBtn);

    if (mRow == createId)
    {
        mTitle = "Создание " + title;
        isChanged = true;
    }
    else
    {
        mTitle = "Редактирование " + title;
        emit ModelDataRequest(mRow);
    }
    setWindowTitle(mTitle);
    setLayout(mainLayout);
    // this->exec();
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
    for (auto i = 0; i < dlgItems.count(); i++)
    {
        auto input = qobject_cast<QLineEdit *>(dlgItems[i]);
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
        setWindowTitle(mTitle + " [ИЗМЕНЕНО]");
    }
}

void ModuleDialog::DataChanged(const QString &str)
{
    Q_UNUSED(str);
    DataChanged();
}

void ModuleDialog::SaveData()
{
    if (isChanged)
    {
        QStringList saved;
        for (auto i = 0; i < dlgItems.count(); i++)
        {
            auto input = qobject_cast<QLineEdit *>(dlgItems[i]);
            if (input != nullptr)
                saved.append(input->text());
        }
        if (mRow == createId)
            emit CreateData(saved, &mRow);
        else
            emit UpdateData(saved, mRow);

        setWindowTitle(mTitle);
        isChanged = false;
    }
}
