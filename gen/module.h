#ifndef MODULEFABRIC_H
#define MODULEFABRIC_H

//#include "../check/abstractcheckdialog.h"
//#include "../config/abstractconfdialog.h"
#include "board.h"

#include <QDialog>
#include <QObject>

class Module : public QObject
{
    Q_OBJECT
public:
    enum DialogTypes
    {
        CHECK = 0,
        CONF = 1,
        TUNE = 2
    };

    explicit Module(QObject *parent = nullptr);

    static Module *createModule(quint32 mtype);
    //    AbstractConfDialog *confDialogBase();
    //    AbstractConfDialog *confDialogMez();
    //    QDialog *tuneDialogBase();
    //    QDialog *tuneDialogMez();
    //    AbstractCheckDialog *checkDialogBase();
    //    AbstractCheckDialog *checkDialogMez();
    QList<QDialog *> dialogs();
    QList<QDialog *> confDialogs();
    void addDialogToList(QDialog *dlg, const QString &name = "");

signals:
    void setWarnColor(QList<bool>);
    void setAlarmColor(QList<bool>);

public slots:

private:
    QList<QDialog *> m_Dialogs;
    /*    QList<QDialog *> m_TuneDialogs;
        QList<QDialog *> m_CheckDialogs; */
    /*    AbstractConfDialog *m_confDialogBase;
        AbstractConfDialog *m_confDialogMez;
        QDialog *m_tuneDialogBase;
        QDialog *m_tuneDialogMez;
        AbstractCheckDialog *m_checkDialogBase;
        AbstractCheckDialog *m_checkDialogMez; */
};

#endif // MODULEFABRIC_H
