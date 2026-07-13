#pragma once

#include <comawidgets/tunetabwidget.h>
#include <comawidgets/udialog.h>
#include <tune/abstracttunedialog.h>

#include <QDomDocument>

#include <functional>

class TuneReporter;

class GeneralTuneDialog : public UDialog
{
    Q_OBJECT
public:
    struct TuneDialogStruct
    {
        QString caption;
        std::function<AbstractTuneDialog *()> factory;
        u8 step = 0;
        AbstractTuneDialog *dialog = nullptr; // создаётся лениво, при первом нажатии кнопки шага
    };

    u8 m_tuneStartStep, m_tuneStepCount;
    QList<TuneDialogStruct> m_dialogList;

    explicit GeneralTuneDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);
    void SetupUI(bool noReport);
    virtual void prepareReport();
    int addWidgetToTabWidget(QWidget *w, const QString &caption);
    void addTuneDialog(const QString &caption, std::function<AbstractTuneDialog *()> factory);
    u8 getTuneStepsCount();

protected:
    TuneReporter *m_reporter;

private:
    TuneTabWidget *m_tuneTabWidget;

public slots:
    void setCalibrButtons();
    void generateReport();
};
