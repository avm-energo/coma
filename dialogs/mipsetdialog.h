#ifndef MIPSETDIALOG_H
#define MIPSETDIALOG_H

#include <QDialog>

class MipSetDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MipSetDialog(QWidget *parent = 0);

signals:

public slots:

private:
    void SetupUI();

private slots:
    void SetMipConPar();
};

#endif // MIPSETDIALOG_H
