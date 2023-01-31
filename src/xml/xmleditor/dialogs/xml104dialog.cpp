#include "xml104dialog.h"

#include <QHash>
#include <cstdint>
#include <variant>

namespace StringData
{
constexpr auto requestTransfering = "Опрос";
constexpr auto cycleTransfering = "Циклическая передача";
constexpr auto changeDataTransfering = "Спорадика (по изменению)";
constexpr auto zeroNum = "0";
constexpr auto oneNum = "1";
constexpr auto threeNum = "3";
}

constexpr auto transTypeIndex = 2;

Xml104Dialog::Xml104Dialog(QWidget *parent) : XmlDialog(parent)
{
}

QString Xml104Dialog::convertStringToType(const QString &namedTransType)
{
    static const QHash<QString, QString> convertMap = {
        { StringData::requestTransfering, StringData::zeroNum },    //
        { StringData::cycleTransfering, StringData::oneNum },       //
        { StringData::changeDataTransfering, StringData::threeNum } //
    };
    return convertMap.value(namedTransType, StringData::zeroNum);
}

int Xml104Dialog::convertTypeToIndex(const QString &transTypeNum)
{
    static const QHash<QString, int> convertMap = {
        { StringData::zeroNum, 0 }, //
        { StringData::oneNum, 1 },  //
        { StringData::threeNum, 2 } //
    };
    return convertMap.value(transTypeNum, 0);
}

void Xml104Dialog::setupUI(QVBoxLayout *mainLayout)
{
    using namespace StringData;
    // Настройки окна (размер, положение)
    setupSizePos(650, 180);
    // Создание слоёв окна
    auto sigIdLayout = new QHBoxLayout;
    auto sigTypeLayout = new QHBoxLayout;
    auto transTypeLayout = new QHBoxLayout;
    auto sigGrpLayout = new QHBoxLayout;
    mTitle += "элемента IEC60870";

    // Виджеты для ID сигнала
    auto sigIdLabel = new QLabel("ID сигнала: ", this);
    auto sigIdInput = new QSpinBox(this);
    sigIdInput->setMinimum(idMin);
    sigIdInput->setMaximum(idMax);
    QObject::connect(sigIdInput, &QSpinBox::textChanged, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    QObject::connect(sigIdInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&Xml104Dialog::dataChanged));
    sigIdLayout->addWidget(sigIdLabel);
    sigIdLayout->addWidget(sigIdInput);
    dlgItems.append(sigIdInput);

    // Виджеты для типа регистра
    auto sigTypeLabel = new QLabel("Тип регистра: ", this);
    auto sigTypeInput = new QLineEdit("", this);
    sigTypeInput->setValidator(new QRegExpValidator(QRegExp("^([1-9][0-9]*|0)"), this));
    QObject::connect(
        sigTypeInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    sigTypeLayout->addWidget(sigTypeLabel);
    sigTypeLayout->addWidget(sigTypeInput);
    dlgItems.append(sigTypeInput);

    // Виджеты для типа передачи
    auto transTypeLabel = new QLabel("Тип передачи: ", this);
    auto transType = new QComboBox(this);
    transType->addItems({ requestTransfering, cycleTransfering, changeDataTransfering });
    transType->setEditable(true);
    transType->setCurrentIndex(0);
    QObject::connect(                                               //
        transType, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&Xml104Dialog::dataChanged));          //
    transTypeLayout->addWidget(transTypeLabel);
    transTypeLayout->addWidget(transType);
    dlgItems.append(transType);

    // Виджеты для описания
    auto sigGroupLabel = new QLabel("Группа сигналов: ", this);
    auto sigGroupInput = new QLineEdit("", this);
    QObject::connect(
        sigGroupInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    sigGrpLayout->addWidget(sigGroupLabel);
    sigGrpLayout->addWidget(sigGroupInput);
    dlgItems.append(sigGroupInput);

    // Добавляем слои на главный слой
    mainLayout->addLayout(sigIdLayout);
    mainLayout->addLayout(sigTypeLayout);
    mainLayout->addLayout(transTypeLayout);
    mainLayout->addLayout(sigGrpLayout);
}

QStringList Xml104Dialog::collectData()
{
    auto retVal = XmlDialog::collectData();
    retVal[transTypeIndex] = convertStringToType(retVal[transTypeIndex]);
    return retVal;
}

void Xml104Dialog::modelDataResponse(const QStringList &response)
{
    XmlDialog::modelDataResponse(response);
    auto transType = std::get<QComboBox *>(dlgItems[transTypeIndex]);
    transType->setCurrentIndex(convertTypeToIndex(response[transTypeIndex]));
    resetChangeState();
}
