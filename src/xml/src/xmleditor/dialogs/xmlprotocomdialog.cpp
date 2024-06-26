#include "xml/xmleditor/dialogs/xmlprotocomdialog.h"

XmlProtocomDialog::XmlProtocomDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlProtocomDialog::setupUI(QVBoxLayout *mainLayout)
{
    // Настройки окна (размер, положение)
    setupSizePos(650, 120);
    // Создание слоёв окна
    auto blockLayout = new QHBoxLayout;
    auto sigIdLayout = new QHBoxLayout;
    m_title += "элемента Protocom";

    // Виджеты для описания
    auto blockLabel = new QLabel("Номер блока: ", this);
    auto blockInput = new QSpinBox(this);
    blockInput->setMinimum(idMin);
    blockInput->setMaximum(idMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(blockInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlProtocomDialog::dataChanged));
#endif
    QObject::connect(blockInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlProtocomDialog::dataChanged));
    blockLayout->addWidget(blockLabel);
    blockLayout->addWidget(blockInput);
    m_dlgItems.append(blockInput);

    // Виджеты для ID сигнала
    auto sigIdLabel = new QLabel("ID сигнала: ", this);
    auto sigIdInput = new QSpinBox(this);
    sigIdInput->setMinimum(idMin);
    sigIdInput->setMaximum(idMax);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(sigIdInput, &QSpinBox::textChanged, this, //
        qOverload<const QString &>(&XmlProtocomDialog::dataChanged));
#endif
    QObject::connect(sigIdInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&XmlProtocomDialog::dataChanged));
    sigIdLayout->addWidget(sigIdLabel);
    sigIdLayout->addWidget(sigIdInput);
    m_dlgItems.append(sigIdInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(blockLayout);
    mainLayout->addLayout(sigIdLayout);
}
