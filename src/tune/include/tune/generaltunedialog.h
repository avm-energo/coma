#pragma once

#include <tune/abstracttunedialog.h>
#include <widgets/tunetabwidget.h>
#include <widgets/udialog.h>

#include <QDomDocument>

class TuneReporter;

class GeneralTuneDialog : public UDialog
{
    Q_OBJECT
public:
    struct TuneDialogStruct
    {
        QString caption;
        AbstractTuneDialog *dialog;
    };

    u8 m_tuneStartStep, m_tuneStepCount;
    QList<TuneDialogStruct> m_dialogList;

    explicit GeneralTuneDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);
    void SetupUI(bool noReport);
    virtual void prepareReport();
    int addWidgetToTabWidget(QWidget *w, const QString &caption);
    void addTuneDialog(TuneDialogStruct &dlgStruct);
    u8 getTuneStepsCount();

protected:
    TuneReporter *m_reporter;

private:
    TuneTabWidget *m_tuneTabWidget;

public slots:
    void setCalibrButtons();
    void generateReport();
};
