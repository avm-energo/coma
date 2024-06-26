#include "startup/abstractstartupdialog.h"

#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
#include <gen/stdfunc.h>
#include <tuple>
#include <widgets/epopup.h>
#include <widgets/wd_func.h>

AbstractStartupDialog::AbstractStartupDialog(Device::CurrentDevice *device, QWidget *parent)
    : UDialog(device, parent), m_updateState(ThereWasNoUpdatesRecently)
{
    m_updateState = ThereWasNoUpdatesRecently;
    m_corNeedsToCheck = NoChecks;
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

    using VoidFunction = std::function<void()>;
    const QList<std::tuple<QString, QString, VoidFunction>> funcs {
        { "Задать все начальные значения", ":/icons/tnapprove.svg", [this]() { SetupCor(); } }, //
        { "Сбросить все начальные значения", ":/icons/tnreset.svg", [this]() { ResetCor(); } }, //
        { "Получить из модуля", ":/icons/tnread.svg", [this]() { GetCorBd(); } },               //
        { "Записать в модуль", ":/icons/tnwrite.svg", [this]() { WriteCor(); } },               //
        { "Прочитать значения из файла", ":/icons/tnload.svg", [this]() { ReadFromFile(); } },  //
        { "Сохранить значения в файл", ":/icons/tnsave.svg", [this]() { SaveToFile(); } }       //
    };

    for (auto &func : funcs)
    {
        auto &toolTip = std::get<0>(func);
        auto &iconPath = std::get<1>(func);
        auto pButton = new QPushButton(group);
        pButton->setObjectName("Hexagon");
        pButton->setIcon(QIcon(iconPath));
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
    for (auto it = m_regMapW.cbegin(); it != m_regMapW.cend(); ++it)
    {
        DataTypes::FloatStruct value { it.key(), *it.value(), DataTypes::Quality::Good };
        values.push_back(QVariant::fromValue(value));
    }
    engine()->currentConnection()->writeCommand(Commands::C_WriteUserValues, values);
    m_corNeedsToCheck = CheckForRegMap; // we should check regs for equality at the next sigs receive
    GetCorBd();
}

void AbstractStartupDialog::GetCorBd()
{
    m_uncheckedRegCount = m_regCountToCheck = m_startupBlockDescription.size / sizeof(float);
    engine()->currentConnection()->reqStartup(m_startupBlockDescription.initStartRegAdr,
        m_startupBlockDescription.size / sizeof(float)); // /4 => float by default
}

void AbstractStartupDialog::SetupCor()
{
    if (checkPassword())
    {
        if (checkStartupValues())
        {
            setSuccessMsg("Начальные значения записаны успешно");
            engine()->currentConnection()->writeCommand(Commands::C_SetStartupValues);
            GetCorBd();
        }
    }
}

void AbstractStartupDialog::ResetCor()
{
    if (checkPassword())
    {
        setSuccessMsg("Начальные значения сброшены успешно");
        engine()->currentConnection()->writeCommand(Commands::C_ClearStartupValues);
        m_corNeedsToCheck = CheckForZeroes; // we should check regs for equality at the next sigs receive
        GetCorBd();
    }
}

void AbstractStartupDialog::sendCommand(Commands cmd, bool value)
{
    if (checkPassword())
    {
        engine()->currentConnection()->writeCommand(cmd, value);
        GetCorBd();
    }
}

void AbstractStartupDialog::updateFloatData(const DataTypes::FloatStruct &fl)
{
    // Игнорируем 4011 т.к. он нам не важен и все чужие регистры тоже игнорируем
    if (fl.sigAdr >= m_regMapR.firstKey() && fl.sigAdr <= m_regMapR.lastKey())
    {
        if (fl.sigQuality != DataTypes::Quality::Good)
            FillBd(QString::number(fl.sigAdr), MAXFLOAT);
        else
            FillBd(QString::number(fl.sigAdr), fl.sigVal);
        float valueToCheck = (m_corNeedsToCheck == CheckForRegMap) ? *(m_regMapR.value(fl.sigAdr)) : 0;
        if (m_corNeedsToCheck != NoChecks)
        {
            if (StdFunc::FloatIsWithinLimits(fl.sigVal, valueToCheck, 0.1))
            {
                --m_uncheckedRegCount;
            }
            if (--m_regCountToCheck <= 0)
            {
                m_corNeedsToCheck = NoChecks;
                // emit corWasChecked(m_uncheckedRegCount);
                // setMessageUponCheck(m_uncheckedRegCount);
            }
        }
    }
}

void AbstractStartupDialog::FillBd(const QString &name, const QString &value)
{
    bool ok;
    double d = value.toDouble(&ok);
    if (ok)
    {
        if (!WDFunc::SetSPBData(this, name, d))
            qDebug() << "Failed to find SpinBox";
    }
    else
        qDebug() << "Failed to convert" << value.toFloat();
}

void AbstractStartupDialog::FillBd(const QString &name, const float value)
{
    if (!WDFunc::SetSPBData(this, name, value))
    {
        qDebug() << "Failed to find SpinBox with name: " << name << " to setup value: " << value;
    }
}

void AbstractStartupDialog::uponInterfaceSetting()
{
    SetupUI();
}

bool AbstractStartupDialog::addReg(quint16 regW, quint16 regR, float *ptr)
{
    Q_ASSERT(!m_regMapW.key(ptr) && "Pointer already exist");
    Q_ASSERT(!m_regMapR.key(ptr) && "Pointer already exist");
    if (m_regMapW.key(ptr))
        return false;
    m_regMapW.insert(regW, ptr);
    m_regMapR.insert(regR, ptr);
    return true;
}

void AbstractStartupDialog::FillCor()
{
    for (auto it = m_regMapR.cbegin(); it != m_regMapR.cend(); ++it)
        if (!WDFunc::SetSPBData(this, QString::number(it.key()), *it.value()))
            qDebug() << "Not found";
}

void AbstractStartupDialog::FillBackCor()
{
    for (auto it = m_regMapR.begin(); it != m_regMapR.end(); ++it)
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
            m_updateState = AnswerWasReceived;
            break;
        case AnswerWasReceived:
            break;
        }
    }
    else
        m_updateState = ThereWasNoUpdatesRecently;
}

// float AbstractStartupDialog::ToFloat(QString text)
//{
//    bool ok;
//    float tmpf;
//    tmpf = text.toFloat(&ok);
//    if (!ok)
//    {
//        qCritical() << "Значение " << text << " не может быть переведено во float";
//        return 0;
//    }
//    return tmpf;
//}

// void AbstractStartupDialog::ErrorRead()
//{
//    QMessageBox::information(this, "Ошибка", "Ошибка чтения");
//}

// void AbstractStartupDialog::setMessageUponCheck(int uncheckedRegCount)
//{
//    if (uncheckedRegCount == 0)
//        EMessageBox::information(this, "Записано успешно");
//    else
//        EMessageBox::warning(this, "Запись не состоялась");
//}
