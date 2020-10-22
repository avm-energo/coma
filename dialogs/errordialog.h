#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

#include "../gen/error.h"

#include <QDialog>
class ErrorProtocolWidget;
class ErrorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ErrorDialog(QWidget *parent = 0);

    void InitDialog(); // initiate dialog with contents from pc.ErMsgPool

signals:

public slots:
    void AddErrMsg(Error::ErMsg msg);

private:
    ErrorProtocolWidget *ErrorWidget;

    void SetupUI();

private slots:
};

#endif // ERRORDIALOG_H
