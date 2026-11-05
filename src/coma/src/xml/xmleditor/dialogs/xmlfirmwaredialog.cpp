#include "xml/xmleditor/dialogs/xmlfirmwaredialog.h"

#include <libavm-widgets/emessagebox.h>

#include <QRegularExpression>

namespace
{
// Тип базовой и мезонинной плат в hex, например a243
const QRegularExpression moduleRegExp("^[0-9a-fA-F]{4}$");
// Версия ВПО в формате major.minor-build, например 0.9-0009
const QRegularExpression versionRegExp(R"(^\d+\.\d+-\d+$)");
} // namespace

XmlFirmwareDialog::XmlFirmwareDialog(QWidget *parent) : XmlDialog(parent) { }

void XmlFirmwareDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(450, 120);
    auto moduleLayout = new QHBoxLayout;
    auto versionLayout = new QHBoxLayout;
    m_title += "версии ВПО модуля";

    // Виджеты для типа модуля
    auto moduleLabel = new QLabel("Модуль (MTypeB MTypeM, например a243): ", this);
    auto moduleInput = new QLineEdit("", this);
    moduleInput->setMaxLength(4);
    QObject::connect(
        moduleInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlFirmwareDialog::dataChanged));
    moduleLayout->addWidget(moduleLabel);
    moduleLayout->addWidget(moduleInput);
    m_dlgItems.append(moduleInput);

    // Виджеты для минимальной версии ВПО
    auto versionLabel = new QLabel("Минимальная версия ВПО (например, 0.9-0009): ", this);
    auto versionInput = new QLineEdit("", this);
    QObject::connect(
        versionInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlFirmwareDialog::dataChanged));
    versionLayout->addWidget(versionLabel);
    versionLayout->addWidget(versionInput);
    m_dlgItems.append(versionInput);

    mainLayout->addLayout(moduleLayout);
    mainLayout->addLayout(versionLayout);
}

bool XmlFirmwareDialog::checkDataBeforeSaving(const QStringList &savedData)
{
    if (savedData.size() < 2 || !moduleRegExp.match(savedData.at(0).trimmed()).hasMatch())
    {
        EMessageBox::warning(this, "Модуль должен состоять из 4 шестнадцатеричных цифр, например a243");
        return false;
    }
    if (!versionRegExp.match(savedData.at(1).trimmed()).hasMatch())
    {
        EMessageBox::warning(this, "Версия ВПО должна быть вида 0.9-0009");
        return false;
    }
    return true;
}
