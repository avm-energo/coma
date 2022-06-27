#ifndef ABSTRACTCORDIALOG_H
#define ABSTRACTCORDIALOG_H

#include "../widgets/udialog.h"

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

    struct StartupBlockStruct
    {
        quint32 initStartRegAdr;
        int num;
        void *block;
        quint32 size;
    };

    explicit AbstractStartupDialog(QWidget *parent = nullptr);
    //    ~AbstractCorDialog();
    void SetStartupBlock(int blocknum, void *block, quint32 blocksize, quint32 startAdr);

    QWidget *buttonWidget();

protected:
    virtual void WriteCor();
    virtual void SetupCor();

    virtual void ResetCor();
    virtual void GetCorBd();

    void updateFloatData(const QVariant &data) override;
    virtual void SaveToFile() = 0;
    virtual void ReadFromFile() = 0;
    void ErrorRead();
    void uponInterfaceSetting() override;
    bool addReg(quint16 reg, float *ptr);
    virtual void FillCor();
    virtual void FillBackCor();

private:
    UpdateStates m_updateState;
    StartupBlockStruct m_startupBlockDescription;
    QMap<quint16, float *> m_regMap;

    float ToFloat(QString text);
    virtual void SetupUI() = 0;
    void FillBd(QWidget *parent, QString Name, QString Value);
    void FillBd(QWidget *parent, QString Name, float Value);

public slots:

    void reqUpdate() override;
};

#endif // ABSTRACTCORDIALOG_H
