#ifndef ERRORPROTOCOLWIDGET_H
#define ERRORPROTOCOLWIDGET_H

#include <QWidget>
struct ErrorMsg;
class ErrorProtocolModel;
class ErrorProtocolWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ErrorProtocolWidget(QWidget *parent = 0);
    ~ErrorProtocolWidget();

    void AddRowToProt(ErrorMsg ermsg);

private:
    ErrorProtocolModel *Model;
};

#endif // ERRORPROTOCOLWIDGET_H
