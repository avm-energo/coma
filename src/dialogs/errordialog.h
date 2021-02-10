#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

#include <QDialog>
class ErrorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ErrorDialog(QWidget *parent = 0);

private:
    void SetupUI();
};

#endif // ERRORDIALOG_H
