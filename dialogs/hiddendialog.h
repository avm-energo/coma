#ifndef HIDDENDIALOG_H
#define HIDDENDIALOG_H

#include <QDialog>
#include <QPaintEvent>
#include <QVBoxLayout>

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

signals:
    void accepted();

private:
    QString BGImage;
    int Type;

    void SetupUI();
    void Fill();
    void SetVersion(quint32 number, QString lename);
    void GetVersion(quint32 &number, QString lename);
    void ToHex(quint32 dec);
    void ToDec(quint32 hex);

private slots:
    void AcceptChanges();

protected:
    void paintEvent(QPaintEvent *e);
};

#endif // HIDDENDIALOG_H
