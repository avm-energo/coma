#include "xmleditalarmdialog.h"

#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QPushButton>
#include <QScreen>

XmlEditAlarmDialog::XmlEditAlarmDialog(XmlSortProxyModel *model, QWidget *parent) : XmlEditDialog(model, parent)
{
}

void XmlEditAlarmDialog::setupUI(QStringList &selectedData)
{
    Q_ASSERT(selectedData.size() == 2);
    // Настройки окна (размер, положение)
    constexpr auto width = 650, height = 120;
    setWindowTitle("Редактирование сигнализации");
    auto centerPoint = QGuiApplication::primaryScreen()->geometry().center();
    setGeometry(centerPoint.x() - width / 2, centerPoint.y() - height / 2, width, height);
    setFixedSize(this->size());

    auto mainLayout = new QVBoxLayout(this);
    auto layout1 = new QHBoxLayout(this);
    auto layout2 = new QHBoxLayout(this);

    auto descLabel = new QLabel("Описание сигнализации: ", this);
    layout1->addWidget(descLabel);
    descInput = new QLineEdit(selectedData[0], this);
    QObject::connect(descInput, &QLineEdit::textEdited, this, &XmlEditAlarmDialog::dataChanged);
    layout1->addWidget(descInput);

    auto addrLabel = new QLabel("Адрес сигнализации: ", this);
    layout2->addWidget(addrLabel);
    addrInput = new QLineEdit(selectedData[1], this);
    addrInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(addrInput, &QLineEdit::textEdited, this, &XmlEditAlarmDialog::dataChanged);
    layout2->addWidget(addrInput);

    mainLayout->addLayout(layout1);
    mainLayout->addLayout(layout2);

    auto saveButton = new QPushButton("Сохранить изменения", this);
    QObject::connect(saveButton, &QPushButton::released, this, &XmlEditAlarmDialog::saveData);
    mainLayout->addWidget(saveButton);

    setLayout(mainLayout);
}

QStringList XmlEditAlarmDialog::collectData()
{
    return { descInput->text(), addrInput->text() };
}

void XmlEditAlarmDialog::dataChanged(const QString &strData)
{
    Q_UNUSED(strData);
    XmlEditDialog::dataChanged();
}
