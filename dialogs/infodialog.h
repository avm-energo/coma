#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QMap>
#include "../config/config.h"

class InfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InfoDialog(QWidget *parent = 0);

    void SetupUI();

signals:

public slots:
    void FillBsi();
    void ClearBsi();

private:
    QMap<int, QString> ModuleNames()
    {
        QMap<int, QString> map;
        map[MTB_A1] = "ПКДН";
        map[MTB_80] = "АВТУК";
        return map;
    }


};

#endif // INFODIALOG_H
