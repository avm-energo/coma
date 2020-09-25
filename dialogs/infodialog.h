#ifndef INFODIALOG_H
#define INFODIALOG_H

#define MTYPE_KTF 0xA287
#define MTYPE_KIV 0xA284
#define MTYPE_KDV 0xA387

#include <QDialog>
#include <QMap>

class InfoDialog : public QDialog
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

private:
};

#endif // INFODIALOG_H
