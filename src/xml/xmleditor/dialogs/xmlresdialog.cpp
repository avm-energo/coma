#include "xmlresdialog.h"

XmlResDialog::XmlResDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlResDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(500, 120);
    // Создание слоёв окна
    auto descLayout = new QHBoxLayout;
    mTitle += "корневого элемента";

    // Виджеты для описания элемента
    auto descLabel = new QLabel("Описание: ", this);
    auto descInput = new QLineEdit("", this);
    QObject::connect(descInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlResDialog::dataChanged));
    descLayout->addWidget(descLabel);
    descLayout->addWidget(descInput);
    dlgItems.append(descInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(descLayout);
}
