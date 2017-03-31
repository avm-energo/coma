#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

#include <QDialog>
#include "../publicclass.h"
#include "../widgets/errorprotocolwidget.h"

class ErrorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ErrorDialog(QWidget *parent = 0);

signals:

public slots:
    void AddErrMsg(publicclass::ermsg msg);

private:
    ErrorProtocolWidget *ErrorWidget;

    void SetupUI();

private slots:

};

#endif // ERRORDIALOG_H
