#pragma once

#include "../widgets/udialog.h"
#include "journaltabwidget.h"

#include <map>

namespace journals
{

class JournalDialog : public UDialog
{
    Q_OBJECT
public:
    JournalDialog(const ModuleSettings &settings, QWidget *parent = nullptr);

private:
    DataTypesProxy proxy;
    std::map<JournalType, JournalTabWidget *> journals;

    void createJournals(const ModuleSettings &settings);
    void setupUI();

private slots:
    void receivedJournalFile(const QVariant &jourData);
};

}
