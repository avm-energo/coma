#include "tune/82/tune82verification.h"

#include <avm-widgets/emessagebox.h>
#include <gen/stdfunc.h>
#include <interfaces/conn/sync_connection.h>
#include <tune/82/verification_offset.h>
#include <tune/mip.h>

#include <QLabel>
#include <QPushButton>

constexpr std::size_t c_iterCount = 21;
constexpr inline std::array<RetomSettings, c_iterCount> c_settings = {
    RetomSettings { 0.0, 60.0, 0.2 },   //
    RetomSettings { 0.0, 60.0, 0.4 },   //
    RetomSettings { 0.0, 60.0, 0.6 },   //
    RetomSettings { 0.0, 60.0, 0.8 },   //
    RetomSettings { 0.0, 60.0, 1.0 },   //
    RetomSettings { 0.0, 60.0, 1.2 },   //
    RetomSettings { 0.0, 60.0, 1.0 },   //
    RetomSettings { 0.0, 60.0, 2.0 },   //
    RetomSettings { 0.0, 60.0, 3.0 },   //
    RetomSettings { 0.0, 60.0, 4.0 },   //
    RetomSettings { 0.0, 60.0, 5.0 },   //
    RetomSettings { 0.0, 60.0, 6.0 },   //
    RetomSettings { 0.0, 72.0, 5.0 },   //
    RetomSettings { 0.0, 48.0, 5.0 },   //
    RetomSettings { 0.0, 36.0, 5.0 },   //
    RetomSettings { 0.0, 24.0, 5.0 },   //
    RetomSettings { 0.0, 12.0, 5.0 },   //
    RetomSettings { 45.0, 60.0, 5.0 },  //
    RetomSettings { 90.0, 60.0, 5.0 },  //
    RetomSettings { 180.0, 60.0, 5.0 }, //
    RetomSettings { 270.0, 60.0, 5.0 }  //
};

Tune82Verification::Tune82Verification(Device::CurrentDevice *device, QWidget *parent)
    : AbstractTuneDialog(device, parent)
    , m_bd1(new Bd182(m_typeM, this))
    , m_mip(new Mip(false, m_typeM, this))
{
    m_bd1->setup(m_device->getUID(), m_sync);
    setupUI();
    reportInit();
}

void Tune82Verification::setTuneFunctions()
{
    addTuneFunc("Сохранение текущей конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("Уменьшение интервала усреднения данных...", &Tune82Verification::setupNFiltrValue);
    addTuneFunc("Поверка...", &Tune82Verification::verification);
    addTuneFunc("Восстановление рабочей конфигурации...", &AbstractTuneDialog::loadWorkConfig);
}

Error::Msg Tune82Verification::setupNFiltrValue()
{
    m_config.setRecord("NFiltr_ID", S2::DWORD(10));
    auto result = m_sync->writeConfigurationSync(m_config.toByteArray());
    StdFunc::Wait(1000);
    return result;
}

void Tune82Verification::showRetomDialog(const RetomSettings &retomData)
{
    auto retomDialog = new QDialog(this);
    retomDialog->setAttribute(Qt::WA_DeleteOnClose);
    retomDialog->setWindowTitle("Настройки РЕТОМ");
    retomDialog->setMinimumSize(400, 200);
    auto layout = new QVBoxLayout;

    QString tempStr( //
        "Задайте на РЕТОМ трёхфазный режим токов и напряжений "
        "(Uabc, Iabc) \nс углами нагрузки по всем фазам %1 град. и частотой 51 Гц;");
    auto phiLabel = new QLabel(tempStr.arg(retomData.phiLoad), retomDialog);
    layout->addWidget(phiLabel);

    if (m_typeM != Device::MezzanineBoard::MTM_81)
    {
        tempStr = "Значения напряжений по фазам %1 В;";
        auto uLabel = new QLabel(tempStr.arg(retomData.voltage), retomDialog);
        layout->addWidget(uLabel);
    }
    if (m_typeM != Device::MezzanineBoard::MTM_83)
    {
        tempStr = "Значения токов по фазам %1 А;";
        auto iLabel = new QLabel(tempStr.arg(retomData.current), retomDialog);
        layout->addWidget(iLabel);
    }

    auto readyBtn = new QPushButton("Готово", retomDialog);
    connect(readyBtn, &QPushButton::clicked, retomDialog, &QDialog::close);
    layout->addWidget(readyBtn);
    auto cancelBtn = new QPushButton("Отмена", retomDialog);
    connect(cancelBtn, &QPushButton::clicked, this, &AbstractTuneDialog::CancelTune);
    connect(cancelBtn, &QPushButton::clicked, retomDialog, &QDialog::close);
    layout->addWidget(cancelBtn);
    retomDialog->setLayout(layout);
    retomDialog->exec();
}

void Tune82Verification::writeMipDataToReport(const MipDataStruct &mipData, const std::size_t iter)
{
    writeReportData("FreqMIP", QString::number(mipData.freqUPhase[0], 'f', 3));
    writeReportData(QString("UA_MIP.%1").arg(iter), QString::number(mipData.uPhase[0], 'f', 3));
    writeReportData(QString("UB_MIP.%1").arg(iter), QString::number(mipData.uPhase[1], 'f', 3));
    writeReportData(QString("UC_MIP.%1").arg(iter), QString::number(mipData.uPhase[2], 'f', 3));
    writeReportData(QString("IA_MIP.%1").arg(iter), QString::number(mipData.iPhase[0], 'f', 3));
    writeReportData(QString("IB_MIP.%1").arg(iter), QString::number(mipData.iPhase[1], 'f', 3));
    writeReportData(QString("IC_MIP.%1").arg(iter), QString::number(mipData.iPhase[2], 'f', 3));
    writeReportData(QString("PhiLA_MIP.%1").arg(iter), QString::number(mipData.phiLoadPhase[0], 'f', 3));
    writeReportData(QString("PhiLB_MIP.%1").arg(iter), QString::number(mipData.phiLoadPhase[1], 'f', 3));
    writeReportData(QString("PhiLC_MIP.%1").arg(iter), QString::number(mipData.phiLoadPhase[2], 'f', 3));
    writeReportData(QString("PhiUab_MIP.%1").arg(iter), QString::number(mipData.phiUab, 'f', 3));
    writeReportData(QString("PhiUbc_MIP.%1").arg(iter), QString::number(mipData.phiUbc, 'f', 3));
}

void Tune82Verification::writeDeviceDataToReport(const Bd182::BlockData &deviceData, const std::size_t iter)
{
    writeReportData(QString("Freq.%1").arg(iter), QString::number(deviceData.Frequency, 'f', 3));
    writeReportData(QString("UA.%1").arg(iter), QString::number(deviceData.IUefNat_filt[0], 'f', 3));
    writeReportData(QString("UB.%1").arg(iter), QString::number(deviceData.IUefNat_filt[1], 'f', 3));
    writeReportData(QString("UC.%1").arg(iter), QString::number(deviceData.IUefNat_filt[2], 'f', 3));
    writeReportData(QString("IA.%1").arg(iter), QString::number(deviceData.IUefNat_filt[3], 'f', 3));
    writeReportData(QString("IB.%1").arg(iter), QString::number(deviceData.IUefNat_filt[4], 'f', 3));
    writeReportData(QString("IC.%1").arg(iter), QString::number(deviceData.IUefNat_filt[5], 'f', 3));
}

void Tune82Verification::writeOffsetDataToReport(const VerificationOffset &offset, const std::size_t iter)
{
    writeReportData(QString("PhiLA.%1").arg(iter), QString::number(offset.phiLoad[0], 'f', 3));
    writeReportData(QString("PhiLB.%1").arg(iter), QString::number(offset.phiLoad[1], 'f', 3));
    writeReportData(QString("PhiLC.%1").arg(iter), QString::number(offset.phiLoad[2], 'f', 3));
    writeReportData(QString("PhiUab.%1").arg(iter), QString::number(offset.phiUab, 'f', 3));
    writeReportData(QString("PhiUbc.%1").arg(iter), QString::number(offset.phiUbc, 'f', 3));
    writeReportData(QString("OffsetF.%1").arg(iter), QString::number(offset.offsetF, 'f', 3));
    writeReportData(QString("OffsetUA.%1").arg(iter), QString::number(offset.offsetU[0], 'f', 3));
    writeReportData(QString("OffsetUB.%1").arg(iter), QString::number(offset.offsetU[1], 'f', 3));
    writeReportData(QString("OffsetUC.%1").arg(iter), QString::number(offset.offsetU[2], 'f', 3));
    writeReportData(QString("OffsetIA.%1").arg(iter), QString::number(offset.offsetI[0], 'f', 3));
    writeReportData(QString("OffsetIB.%1").arg(iter), QString::number(offset.offsetI[1], 'f', 3));
    writeReportData(QString("OffsetIC.%1").arg(iter), QString::number(offset.offsetI[2], 'f', 3));
    writeReportData(QString("OffsetPhiloadA.%1").arg(iter), QString::number(offset.offsetPhiLoad[0], 'f', 3));
    writeReportData(QString("OffsetPhiloadB.%1").arg(iter), QString::number(offset.offsetPhiLoad[1], 'f', 3));
    writeReportData(QString("OffsetPhiloadC.%1").arg(iter), QString::number(offset.offsetPhiLoad[2], 'f', 3));
    writeReportData(QString("OffsetPhiUAB.%1").arg(iter), QString::number(offset.offsetPhiUab, 'f', 3));
    writeReportData(QString("OffsetPhiUBC.%1").arg(iter), QString::number(offset.offsetPhiUbc, 'f', 3));
}

void Tune82Verification::reportInit()
{
    StdFunc::ClearCancel();
    if (!s_reportData.empty())
        s_reportData.clear();

    writeReportData("Organization", "ООО АВМ-Энерго");
    writeReportData("Day", QDateTime::currentDateTime().toString("dd"));
    writeReportData("Month", QDateTime::currentDateTime().toString("MM"));
    writeReportData("Yr", QDateTime::currentDateTime().toString("yy"));
    writeReportData("Serial", QString::number(m_device->bsi().SerialNum, 16));
    writeReportData("SerialB", QString::number(m_device->bsi().SerialNumB, 16));
    writeReportData("SerialM", QString::number(m_device->bsi().SerialNumM, 16));
    writeReportData("HardwareB", StdFunc::VerToStr(m_device->bsi().HwverB));
    writeReportData("HardwareM", StdFunc::VerToStr(m_device->bsi().HwverM));
    writeReportData("Software", StdFunc::VerToStr(m_device->bsi().Fwver));
}

Error::Msg Tune82Verification::verification()
{
    bool ok;
    float i2nom = 0.0;

    MipDataStruct mipData { 0 };
    Bd182::BlockData deviceData { 0 };
    VerificationOffset offsetData { 0 };
    RetomSettings retomData { 0 };

    for (std::size_t iter = 0; iter < c_iterCount; ++iter)
    {
        if (iter == 0)
        {
            i2nom = 1.0;
            if (setCurrentsTo(i2nom) != Error::Msg::NoError)
                return Error::Msg::GeneralError;
        }
        if (iter == 6)
        {
            i2nom = 5.0;
            if (setCurrentsTo(i2nom) != Error::Msg::NoError)
                return Error::Msg::GeneralError;
        }

        retomData = c_settings[iter];
        showRetomDialog(retomData);
        if (StdFunc::IsCancelled())
            return Error::Msg::GeneralError;

        StdFunc::Wait(1000);
        mipData = m_mip->takeOneMeasurement(ok);
        if (!ok)
            return Error::Msg::GeneralError;
        m_bd1->readBlockFromModule();
        deviceData = *(m_bd1->data());

        QCoreApplication::processEvents();
        offsetData.update(mipData, deviceData);
        if (checkMeasuredDataForCorrectValues(mipData, retomData) != Error::Msg::NoError)
        {
            if (EMessageBox::next(this, "Проверьте правильность задания значений на РЕТОМ"))
            {
                --iter;
                continue;
            }
            else
                return Error::Msg::GeneralError;
        }
        writeMipDataToReport(mipData, iter);
        writeDeviceDataToReport(deviceData, iter);
        writeOffsetDataToReport(offsetData, iter);
    }
    return Error::Msg::NoError;
}

Error::Msg Tune82Verification::checkMeasuredDataForCorrectValues(
    const MipDataStruct &mipData, const RetomSettings &retomData)
{
    if (!StdFunc::FloatIsWithinLimits(mipData.freqUPhase[0], 51.0, 0.1))
        return Error::Msg::GeneralError;
    for (int i = 0; i < 3; ++i)
    {
        if (!StdFunc::FloatIsWithinLimits(mipData.uPhase[i], retomData.voltage, 2))
            return Error::Msg::GeneralError;
        if (!StdFunc::FloatIsWithinLimits(mipData.iPhase[i], retomData.current, 0.2))
            return Error::Msg::GeneralError;
        float retPhiLoad = (retomData.phiLoad >= 180) ? (360 - retomData.phiLoad) : -retomData.phiLoad;
        if (!StdFunc::FloatIsWithinLimits(mipData.phiLoadPhase[i], retPhiLoad, 5))
            return Error::Msg::GeneralError;
    }
    return Error::Msg::NoError;
}
