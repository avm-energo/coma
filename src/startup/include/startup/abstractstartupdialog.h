#pragma once

#include <widgets/udialog.h>

class AbstractStartupDialog : public UDialog
{
    Q_OBJECT
public:
    enum UpdateStates
    {
        ThereWasNoUpdatesRecently = 0,
        QueryWasInitiated = 1,
        AnswerWasReceived = 2
    };

    enum CheckStyle
    {
        NoChecks = 0,
        CheckForRegMap = 1,
        CheckForZeroes = 2
    };

    struct StartupBlockStruct
    {
        quint32 initStartRegAdr;
        int num;
        void *block;
        quint32 size;
    };

    explicit AbstractStartupDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);
    void SetStartupBlock(int blocknum, void *block, quint32 blocksize, quint32 startAdr);
    QWidget *buttonWidget();

protected:
    void updateFloatData(const DataTypes::FloatStruct &fl) override;
    void uponInterfaceSetting() override;

    void WriteCor();
    void SetupCor();
    void ResetCor();
    void GetCorBd();
    void FillCor();
    void FillBackCor();

    void sendCommand(Commands cmd, bool value = true);
    bool addReg(quint16 regW, quint16 regR, float *ptr);

    virtual void SaveToFile() = 0;
    virtual void ReadFromFile() = 0;
    virtual bool checkStartupValues() = 0;

    // void ErrorRead();
    // float ToFloat(QString text);
    // void corWasChecked(int uncheckedRegCount);
    // void setMessageUponCheck(int uncheckedRegCount);

private:
    UpdateStates m_updateState;
    StartupBlockStruct m_startupBlockDescription;
    QMap<quint16, float *> m_regMapW, m_regMapR;
    CheckStyle m_corNeedsToCheck; // flag indicating that we should check corRegs from module to be
                                  // equal to these stored in m_regMap
    int m_regCountToCheck, m_uncheckedRegCount;

    virtual void SetupUI() = 0;
    void FillBd(const QString &name, const QString &value);
    void FillBd(const QString &name, const float value);

public slots:
    void reqUpdate() override;
};
