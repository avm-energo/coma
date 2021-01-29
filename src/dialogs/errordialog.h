#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

#include <QDialog>
struct ErrorMsg;
class ErrorProtocolWidget;
class ErrorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ErrorDialog(QWidget *parent = 0);

    void AddErrMsg(ErrorMsg);

private:
    ErrorProtocolWidget *ErrorWidget;

    void SetupUI();
};

#endif // ERRORDIALOG_H
