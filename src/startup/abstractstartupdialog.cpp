#include "abstractstartupdialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/helper.h"
#include "../gen/stdfunc.h"
#include "../widgets/etableview.h"
#include "../widgets/wd_func.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
namespace crypto
{
static constexpr char hash[] = "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc";
static constexpr char name[] = "startHash";
}
AbstractStartupDialog::AbstractStartupDialog(QWidget *parent) : UDialog(crypto::hash, crypto::name, parent)
{
    m_updateState = ThereWasNoUpdatesRecently;

    setSuccessMsg("Стартовые значения записаны успешно");
}

void AbstractStartupDialog::SetStartupBlock(int blocknum, void *block, quint32 blocksize, quint32 startAdr)
{
    m_startupBlockDescription.num = blocknum;
    m_startupBlockDescription.block = block;
    m_startupBlockDescription.size = blocksize;
    m_startupBlockDescription.initStartRegAdr = startAdr;
}

QWidget *AbstractStartupDialog::buttonWidget()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QDialogButtonBox *group = new QDialogButtonBox;
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");

    const QList<QPair<QPair<QString, QIcon>, std::function<void()>>> funcs {
        { { "Задать начальные значения", QIcon(":/icons/tnapprove.svg") }, [this]() { SetupCor(); } },    //
        { { "Сбросить начальные значения", QIcon(":/icons/tnreset.svg") }, [this]() { ResetCor(); } },    //
        { { "Получить из " + tmps, QIcon(":/icons/tnread.svg") }, [this]() { GetCorBd(); } },             //
        { { "Записать в модуль", QIcon(":/icons/tnwrite.svg") }, [this]() { WriteCor(); } },              //
        { { "Прочитать значения из файла", QIcon(":/icons/tnload.svg") }, [this]() { ReadFromFile(); } }, //
        { { "Сохранить значения в файл", QIcon(":/icons/tnsave.svg") }, [this]() { SaveToFile(); } }      //
    };

    for (auto &i : funcs)
    {
        const QIcon &icon = i.first.second;
        const QString &toolTip = i.first.first;
        QPushButton *pb = new QPushButton();
        pb->setObjectName("Hexagon");
        pb->setIcon(icon);

        pb->setToolTip(toolTip);
        pb->setMinimumSize(50, 50);
        pb->setIconSize(QSize(50, 50));

        connect(pb, &QAbstractButton::clicked, this, i.second);

        group->addButton(pb, QDialogButtonBox::ActionRole);
    }
    group->setCenterButtons(true);
    lyout->addWidget(group);

    w->setLayout(lyout);
    return w;
}

void AbstractStartupDialog::WriteCor()
{
    if (!checkPassword())
        return;
    FillBackCor();
    QVariantList values;
    for (decltype(m_regMap)::const_iterator it = m_regMap.cbegin(); it != m_regMap.cend(); ++it)
    {
        DataTypes::FloatStruct value { it.key(), *it.value() };
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

void AbstractStartupDialog::updateFloatData(const DataTypes::FloatStruct &fl)
{
    if (!updatesEnabled())
        return;
    // Игнорируем 4011 т.к. он нам не важен и все чужие регистры тоже игнорируем
    if (fl.sigAdr >= m_regMap.firstKey() && fl.sigAdr <= m_regMap.lastKey())
        FillBd(this, QString::number(fl.sigAdr), fl.sigVal);
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
