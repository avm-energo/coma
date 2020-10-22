#ifndef ERRORPROTOCOLWIDGET_H
#define ERRORPROTOCOLWIDGET_H

#include "../gen/error.h"

#include <QWidget>
class ErrorProtocolModel;
class ErrorProtocolWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ErrorProtocolWidget(QWidget *parent = 0);
    ~ErrorProtocolWidget();

    void AddRowToProt(Error::ErMsg ermsg);

private:
    ErrorProtocolModel *Model;
};

#endif // ERRORPROTOCOLWIDGET_H
