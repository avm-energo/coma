#include "xmlmeasjourdialog.h"

XmlMeasJourDialog::XmlMeasJourDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlMeasJourDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(500, 80);
    // Создание слоёв окна
    auto headLayout = new QHBoxLayout;
    mTitle += "события журнала измерений";

    // Виджеты для описания события
    auto headLabel = new QLabel("Заголовок события: ", this);
    auto headInput = new QLineEdit("", this);
    QObject::connect(
        headInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&XmlMeasJourDialog::dataChanged));
    headLayout->addWidget(headLabel);
    headLayout->addWidget(headInput);
    dlgItems.append(headInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(headLayout);
}
