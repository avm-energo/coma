#include "abstractstartupdialog.h"

#include "../dialogs/keypressdialog.h"
#include "../module/board.h"
#include "../widgets/wd_func.h"

#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
#include <tuple>

//#include "../widgets/etableview.h"
//#include <QButtonGroup>
//#include <QCheckBox>
//#include <QComboBox>
//#include <QCoreApplication>
//#include <QDebug>
//#include <QDoubleSpinBox>
//#include <QFileDialog>
//#include <QGridLayout>
//#include <QGroupBox>
//#include <QLabel>
//#include <QLineEdit>
//#include <QSpinBox>
//#include <gen/colors.h>
//#include <gen/datatypes.h>
//#include <gen/helper.h>
//#include <gen/stdfunc.h>

namespace crypto
{
static constexpr char hash[] = "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc";
static constexpr char name[] = "startHash";
}

AbstractStartupDialog::AbstractStartupDialog(QWidget *parent)
    : UDialog(crypto::hash, crypto::name, parent) //
    , m_updateState(ThereWasNoUpdatesRecently)
{
    m_updateState = ThereWasNoUpdatesRecently;
    setSuccessMsg("Стартовые значения записаны успешно");
}

void AbstractStartupDialog::SetStartupBlock(int blocknum, void *block, quint32 blocksize, quint32 startAdr)
{
    m_startupBlockDescription = { startAdr, blocknum, block, blocksize };
}

QWidget *AbstractStartupDialog::buttonWidget()
{
    auto widget = new QWidget;
    auto layout = new QVBoxLayout;
    auto group = new QDialogButtonBox(widget);
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");

    using VoidFunction = std::function<void()>;
    const QList<std::tuple<QString, QIcon, VoidFunction>> funcs {
        { "Задать начальные значения", QIcon(":/icons/tnapprove.svg"), [this]() { SetupCor(); } },    //
        { "Сбросить начальные значения", QIcon(":/icons/tnreset.svg"), [this]() { ResetCor(); } },    //
        { "Получить из " + tmps, QIcon(":/icons/tnread.svg"), [this]() { GetCorBd(); } },             //
        { "Записать в модуль", QIcon(":/icons/tnwrite.svg"), [this]() { WriteCor(); } },              //
        { "Прочитать значения из файла", QIcon(":/icons/tnload.svg"), [this]() { ReadFromFile(); } }, //
        { "Сохранить значения в файл", QIcon(":/icons/tnsave.svg"), [this]() { SaveToFile(); } }      //
    };

    for (auto &func : funcs)
    {
        const auto &toolTip = std::get<0>(func);
        const auto &icon = std::get<1>(func);
        auto pButton = new QPushButton(group);
        pButton->setObjectName("Hexagon");
        pButton->setIcon(icon);
        pButton->setToolTip(toolTip);
        pButton->setMinimumSize(50, 50);
        pButton->setIconSize(QSize(50, 50));
        connect(pButton, &QAbstractButton::clicked, this, std::get<2>(func));
        group->addButton(pButton, QDialogButtonBox::ActionRole);
    }

    group->setCenterButtons(true);
    layout->addWidget(group);
    widget->setLayout(layout);
    return widget;
}

void AbstractStartupDialog::WriteCor()
{
    if (!checkPassword())
        return;
    FillBackCor();
    QVariantList values;
    for (auto it = m_regMap.cbegin(); it != m_regMap.cend(); ++it)
    {
        DataTypes::FloatStruct value { it.key(), *it.value(), DataTypes::Quality::Good };
        values.push_back(QVariant::fromValue(value));
    }
    BaseInterface::iface()->writeCommand(Queries::QC_WriteUserValues, values);
    GetCorBd();
}

void AbstractStartupDialog::GetCorBd()
{
    BaseInterface::iface()->reqStartup(m_startupBlockDescription.initStartRegAdr,
        m_startupBlockDescription.size / sizeof(float)); // /4 => float by default
}

void AbstractStartupDialog::ResetCor()
{
    if (checkPassword())
    {
        BaseInterface::iface()->writeCommand(Queries::QC_ClearStartupValues);
        GetCorBd();
    }
}

float AbstractStartupDialog::ToFloat(QString text)
{
    bool ok;
    float tmpf;
    tmpf = text.toFloat(&ok);
    if (!ok)
    {
        qCritical() << "Значение " << text << " не может быть переведено во float";
        return 0;
    }
    return tmpf;
}

// void AbstractStartupDialog::updateFloatData(const DataTypes::FloatStruct &fl)
void AbstractStartupDialog::updateFloatData(const DataTypes::FloatStruct &fl)
{
    // Игнорируем 4011 т.к. он нам не важен и все чужие регистры тоже игнорируем
    if (fl.sigAdr >= m_regMap.firstKey() && fl.sigAdr <= m_regMap.lastKey())
    {
        if (fl.sigQuality != 192)
            FillBd(this, QString::number(fl.sigAdr), "***");
        else
            FillBd(this, QString::number(fl.sigAdr), fl.sigVal);
    }
}

void AbstractStartupDialog::FillBd(QWidget *parent, QString Name, QString Value)
{
    bool ok;
    double d = Value.toDouble(&ok);
    if (ok)
    {
        if (!WDFunc::SetSPBData(parent, Name, d))
            qDebug() << "Failed to find SpinBox";
    }
    else
        qDebug() << "Failed to convert" << Value.toFloat();
}

void AbstractStartupDialog::FillBd(QWidget *parent, QString Name, float Value)
{
    if (!WDFunc::SetSPBData(parent, Name, Value))
    {
        qDebug() << "Failed to find SpinBox with name:" << Name << "and parent:" << parent->objectName()
                 << "to setup value: " << Value;
    }
}

void AbstractStartupDialog::SetupCor()
{
    if (checkPassword())
    {
        BaseInterface::iface()->writeCommand(Queries::QC_SetStartupValues);
        GetCorBd();
    }
}

void AbstractStartupDialog::ErrorRead()
{
    QMessageBox::information(this, "Ошибка", "Ошибка чтения");
}

void AbstractStartupDialog::uponInterfaceSetting()
{
    SetupUI();
}

bool AbstractStartupDialog::addReg(quint16 reg, float *ptr)
{
    Q_ASSERT(!m_regMap.key(ptr) && "Pointer already exist");
    if (m_regMap.key(ptr))
        return false;
    m_regMap.insert(reg, ptr);
    return true;
}

void AbstractStartupDialog::FillCor()
{
    for (decltype(m_regMap)::const_iterator it = m_regMap.cbegin(); it != m_regMap.cend(); ++it)
        if (!WDFunc::SetSPBData(this, QString::number(it.key()), *it.value()))
            qDebug() << "Not found";
}

void AbstractStartupDialog::FillBackCor()
{
    for (decltype(m_regMap)::iterator it = m_regMap.begin(); it != m_regMap.end(); ++it)
        if (!WDFunc::SPBData(this, QString::number(it.key()), *it.value()))
            qDebug() << "Not found";
}

void AbstractStartupDialog::reqUpdate()
{
    if (updatesEnabled())
    {
        switch (m_updateState)
        {
        case ThereWasNoUpdatesRecently:
            GetCorBd();
            m_updateState = QueryWasInitiated;
            break;
        case QueryWasInitiated:
        {
            //            updateFloatData();
            m_updateState = AnswerWasReceived;
            break;
        }
        break;
        case AnswerWasReceived:
            //            FillCor();
            //            m_updateState = ThereWasNoUpdatesRecently;
            break;
        }
    }
    else
        m_updateState = ThereWasNoUpdatesRecently;
}
