#ifndef INFODIALOG_H
#define INFODIALOG_H

#include "../widgets/udialog.h"

#include <QMap>

class InfoDialog : public UDialog
{
    Q_OBJECT
public:
    explicit InfoDialog(QWidget *parent = nullptr);

    void SetupUI();
    void FillBsi();
    void uponInterfaceSetting();

signals:

public slots:
    void ClearBsi();
    void reqUpdate();
    void sync();

private:
    bool m_oneShotUpdateFlag;
};

#endif // INFODIALOG_H
