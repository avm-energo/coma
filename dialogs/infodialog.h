#ifndef INFODIALOG_H
#define INFODIALOG_H

#define MTYPE_KTF 0xA287
#define MTYPE_KIV 0xA284
#define MTYPE_KDV 0xA387

#include "../widgets/udialog.h"

#include <QMap>

class InfoDialog : public UDialog
{
    Q_OBJECT
public:
    explicit InfoDialog(QWidget *parent = nullptr);

    QMap<quint16, QString> Inf;

    void SetupUI();
    void FillBsi();

signals:

public slots:
    void ClearBsi();
    void reqUpdate();
    void update();

private:
    bool m_oneShotUpdateFlag;
};

#endif // INFODIALOG_H
