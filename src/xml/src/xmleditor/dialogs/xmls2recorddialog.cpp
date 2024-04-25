#include "xml/xmleditor/dialogs/xmls2recorddialog.h"

const QStringList XmlS2RecordDialog::s_dataTypes {
    "BYTE", "BYTE[4]", "BYTE[6]", "BYTE[8]", "BYTE[16]", "BYTE[32]",       //
    "WORD", "WORD[4]", "WORD[6]", "WORD[8]", "WORD[16]", "WORD[32]",       //
    "DWORD", "DWORD[4]", "DWORD[6]", "DWORD[8]", "DWORD[16]", "DWORD[32]", //
    "FLOAT", "FLOAT[2]", "FLOAT[3]", "FLOAT[4]", "FLOAT[6]", "FLOAT[8]",   //
    "CONF_DENS", "CONF_DENS[3]", "BYTE[]", "INT32",                        //
};

XmlS2RecordDialog::XmlS2RecordDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlS2RecordDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 300);

    // Создание слоёв окна
    auto idLayout = new QHBoxLayout;
    auto nameLayout = new QHBoxLayout;
    auto datatypeLayout = new QHBoxLayout;
    auto descLayout = new QHBoxLayout;
    m_title += "описания S2 записи";

    // Виджеты для ID записи
    auto idLabel = new QLabel("ID записи: ", this);
    auto idInput = new QSpinBox(this);
    idInput->setMinimum(idMin);
    idInput->setMaximum(idMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(idInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
#endif
    QObject::connect(idInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlS2RecordDialog::dataChanged));
    idLayout->addWidget(idLabel);
    idLayout->addWidget(idInput);
    mainLayout->addLayout(idLayout);
    m_dlgItems.append(idInput);

    // Виджеты для имени записи
    auto nameLabel = new QLabel("Имя записи: ", this);
    auto nameInput = new QLineEdit("", this);
    QObject::connect(nameInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameInput);
    mainLayout->addLayout(nameLayout);
    m_dlgItems.append(nameInput);

    // Виджеты для типа данных записи
    auto datatypeLabel = new QLabel("Тип передачи: ", this);
    auto datatypeInput = new QComboBox(this);
    datatypeInput->addItems(s_dataTypes);
    datatypeInput->setCurrentIndex(0);
    QObject::connect(                                                   //
        datatypeInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&XmlS2RecordDialog::dataChanged));         //
    datatypeLayout->addWidget(datatypeLabel);
    datatypeLayout->addWidget(datatypeInput);
    mainLayout->addLayout(datatypeLayout);
    m_dlgItems.append(datatypeInput);

    // Виджеты для описания записи
    auto descLabel = new QLabel("Описание: ", this);
    auto descInput = new QLineEdit("", this);
    QObject::connect(descInput, &QLineEdit::textEdited, this, //
        qOverload<const QString &>(&XmlS2RecordDialog::dataChanged));
    descLayout->addWidget(descLabel);
    descLayout->addWidget(descInput);
    mainLayout->addLayout(descLayout);
    m_dlgItems.append(descInput);
}
