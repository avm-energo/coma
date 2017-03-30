#ifndef HIDDENDIALOG_H
#define HIDDENDIALOG_H

#include <QDialog>
#include <QPaintEvent>

class HiddenDialog : public QDialog
{
    Q_OBJECT
public:
    enum BM
    {
        BYMY,
        BNMY,
        BYMN,
        BNMN
    };

    explicit HiddenDialog(int type, QWidget *parent = 0);

private:
    QString BGImage;

    void SetupUI();

protected:
    void paintEvent(QPaintEvent *e);
};

#endif // HIDDENDIALOG_H
