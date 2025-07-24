#pragma once

#include <QWidget>

class ErrorProtocolWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ErrorProtocolWidget(QWidget *parent = nullptr);
    ~ErrorProtocolWidget();
};
