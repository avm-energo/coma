#include "xml/xmleditor/dialogs/moduledialog.h"

#include <widgets/chbfunc.h>
#include <widgets/lblfunc.h>
#include <widgets/lefunc.h>

#include <QGuiApplication>
#include <QScreen>

constexpr auto bTypeIndex = 1;
constexpr auto versionIndex = 3;

ModuleDialog::ModuleDialog(QWidget *parent) : XmlDialog(parent) { }

void ModuleDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(380, 220);
    m_elements = { { "Название", "nameInput", WidgetTypes::LineEdit }, //
        { "База: ", "bTypeInput", WidgetTypes::LineEdit },             //
        { "Мезонин: ", "mTypeInput", WidgetTypes::LineEdit },          //
        { "Версия", "verInput", WidgetTypes::LineEdit },               //
        { "Файл XML", "XMLFile", WidgetTypes::NoWidget },
        { "Отдельный модуль в коробке", "isBoxModule", WidgetTypes::CheckBox },
        { "Часы RTC есть", "isRTCExist", WidgetTypes::CheckBox } };
    m_title += "модуля";
    for (const auto &item : std::as_const(m_elements))
    {
        auto labelText = item.title;
        auto itemName = item.name;
        switch (item.type)
        {
        case WidgetTypes::LineEdit:
        {
            auto labelItem = LBLFunc::New(this, labelText, itemName + "Label");
            auto itemLayout = new QHBoxLayout;
            itemLayout->addWidget(labelItem);
            auto inputItem = LEFunc::New(this, itemName);
            QObject::connect(
                inputItem, &QLineEdit::textEdited, this, qOverload<const QString &>(&ModuleDialog::dataChanged));
            itemLayout->addWidget(inputItem);
            mainLayout->addLayout(itemLayout);
            m_dlgItems.append(inputItem);
            break;
        }
        case WidgetTypes::CheckBox:
        {
            auto labelItem = LBLFunc::New(this, labelText, itemName + "Label");
            auto itemLayout = new QHBoxLayout;
            itemLayout->addWidget(labelItem, 10);
            auto inputItem = ChBFunc::New(this, itemName, "");
#if (QT_VERSION < QT_VERSION_CHECK(6, 8, 2))
            QObject::connect(inputItem, &QCheckBox::stateChanged, this, &ModuleDialog::dataChanged);
#else
            QObject::connect(
                inputItem, &QCheckBox::checkStateChanged, this, qOverload<Qt::CheckState>(&ModuleDialog::dataChanged));
#endif
            itemLayout->addWidget(inputItem, 1);
            mainLayout->addLayout(itemLayout);
            m_dlgItems.append(inputItem);
            break;
        }
        default: // NoWidget
            auto inputItem = LEFunc::New(this, itemName, "");
            inputItem->setVisible(false);
            m_dlgItems.append(inputItem);
            break;
        }
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
