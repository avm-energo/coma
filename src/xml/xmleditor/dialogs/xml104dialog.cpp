#include "xml104dialog.h"

#include <QHash>
#include <cstdint>
#include <gen/stdfunc.h>
#include <variant>

namespace StringData
{
constexpr auto requestTransfering = "Опрос";
constexpr auto cycleTransfering = "Циклическая передача";
constexpr auto changeDataTransfering = "Спорадика (по изменению)";
constexpr auto none = "None";

constexpr auto zeroNum = "0";
constexpr auto oneNum = "1";
constexpr auto threeNum = "3";
constexpr auto noneNum = "";
}

constexpr auto transTypeIndex = 2;

Xml104Dialog::Xml104Dialog(QWidget *parent) : XmlDialog(parent)
{
}

QString Xml104Dialog::convertStringToType(const QString &namedTransType)
{
    using namespace StringData;
    static const QHash<QString, QString> convertMap = {
        { requestTransfering, zeroNum },     //
        { cycleTransfering, oneNum },        //
        { changeDataTransfering, threeNum }, //
        { none, noneNum }                    //
    };
    return convertMap.value(namedTransType, zeroNum);
}

int Xml104Dialog::convertTypeToIndex(const QString &transTypeNum)
{
    using namespace StringData;
    static const QHash<QString, int> convertMap = {
        { zeroNum, 0 },  //
        { oneNum, 1 },   //
        { threeNum, 2 }, //
        { noneNum, 3 }   //
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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QObject::connect(sigIdInput, &QSpinBox::textChanged, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
#endif
    QObject::connect(sigIdInput, qOverload<int>(&QSpinBox::valueChanged), //
        this, qOverload<int>(&Xml104Dialog::dataChanged));
    sigIdLayout->addWidget(sigIdLabel);
    sigIdLayout->addWidget(sigIdInput);
    dlgItems.append(sigIdInput);

    // Виджеты для типа регистра
    auto sigTypeLabel = new QLabel("Тип регистра: ", this);
    auto sigTypeInput = new QLineEdit("", this);
    sigTypeInput->setValidator(StdFunc::getRegExpValidator("^([1-9][0-9]*|0)", this));
    QObject::connect(
        sigTypeInput, &QLineEdit::textEdited, this, qOverload<const QString &>(&Xml104Dialog::dataChanged));
    sigTypeLayout->addWidget(sigTypeLabel);
    sigTypeLayout->addWidget(sigTypeInput);
    dlgItems.append(sigTypeInput);

    // Виджеты для типа передачи
    auto transTypeLabel = new QLabel("Тип передачи: ", this);
    auto transTypeInput = new QComboBox(this);
    transTypeInput->addItems({ requestTransfering, cycleTransfering, changeDataTransfering, none });
    transTypeInput->setCurrentIndex(0);
    QObject::connect(                                                    //
        transTypeInput, qOverload<int>(&QComboBox::currentIndexChanged), //
        this, qOverload<int>(&Xml104Dialog::dataChanged));               //
    transTypeLayout->addWidget(transTypeLabel);
    transTypeLayout->addWidget(transTypeInput);
    dlgItems.append(transTypeInput);

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
    auto transTypeInput = std::get<QComboBox *>(dlgItems[transTypeIndex]);
    transTypeInput->setCurrentIndex(convertTypeToIndex(response[transTypeIndex]));
    resetChangeState();
}
