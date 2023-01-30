#pragma once

#include "../widgets/udialog.h"
#include "dialogcreator.h"

#include <QListWidget>
#include <QPair>
#include <QStackedWidget>

class DialogManager : public UDialog
{
    Q_OBJECT
private:
    int curDialogIndex;
    // QList<UDialog *> mDialogs;
    UniquePointer<DialogCreator> mDlgCreator;
    UniquePointer<QStackedWidget> mWorkspace;
    UniquePointer<QListWidget> mSidebar;

    void init();
    void hideUI();
    void showUI();

public:
    DialogManager(const ModuleSettings &settings, QWidget *parent = nullptr);
    QPair<QListWidget *, QStackedWidget *> getUI();
    void setupUI(const AppConfiguration appCfg, const QSize size);

public slots:
    void reqUpdate() override;
    void dialogChanged(int newIndex);
    void clearDialogs();
};
