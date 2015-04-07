#ifndef S_TQCHECKBOX_H
#define S_TQCHECKBOX_H

#include <QCheckBox>
#include <QVariant>

class s_tqCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    explicit s_tqCheckBox(QWidget *parent = 0);
    QVariant getAData();
    void setAData(QVariant dat);

signals:
    void statechanged(int, s_tqCheckBox *);

public slots:

private slots:
    void changestate(int);

private:
    QVariant adata;
};

#endif // S_TQCHECKBOX_H
