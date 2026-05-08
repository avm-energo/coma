#include "xml/xmleditor/dialogs/xmlbsirecorddialog.h"

XmlBsiRecordDialog::XmlBsiRecordDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlBsiRecordDialog::setupUI(QVBoxLayout *mainLayout)
{
    setupSizePos(650, 190);
    m_title += "записи BSI";

    auto nameLayout = new QHBoxLayout;
    auto descLayout = new QHBoxLayout;
    auto reprLayout = new QHBoxLayout;
    auto offsetLayout = new QHBoxLayout;

    // Имя поля (objectName для лейбла значения)
    auto nameLabel = new QLabel("Имя поля: ", this);
    auto nameInput = new QLineEdit("", this);
    QObject::connect(nameInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlBsiRecordDialog::dataChanged));
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameInput);
    m_dlgItems.append(nameInput);

    // Описание (текст лейбла слева)
    auto descLabel = new QLabel("Описание: ", this);
    auto descInput = new QLineEdit("", this);
    QObject::connect(descInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlBsiRecordDialog::dataChanged));
    descLayout->addWidget(descLabel);
    descLayout->addWidget(descInput);
    m_dlgItems.append(descInput);

    // Тип вывода (ViewTypes enum из avm-widgets/viewtypewidget.h)
    auto reprLabel = new QLabel("Тип вывода: ", this);
    auto reprInput = new QComboBox(this);
    reprInput->addItems({ "String", "IpControl", "SerialNumber", "Version", "Unknown" });
    reprInput->setCurrentIndex(0);
    QObject::connect(reprInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlBsiRecordDialog::dataChanged));
    reprLayout->addWidget(reprLabel);
    reprLayout->addWidget(reprInput);
    m_dlgItems.append(reprInput);

    // Смещение (индекс в массиве BSI/BsiExt)
    auto offsetLabel = new QLabel("Смещение: ", this);
    auto offsetInput = new QSpinBox(this);
    offsetInput->setMinimum(0);
    offsetInput->setMaximum(255);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(offsetInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlBsiRecordDialog::dataChanged));
#endif
    QObject::connect(offsetInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlBsiRecordDialog::dataChanged));
    offsetLayout->addWidget(offsetLabel);
    offsetLayout->addWidget(offsetInput);
    m_dlgItems.append(offsetInput);

    mainLayout->addLayout(nameLayout);
    mainLayout->addLayout(descLayout);
    mainLayout->addLayout(reprLayout);
    mainLayout->addLayout(offsetLayout);
}
