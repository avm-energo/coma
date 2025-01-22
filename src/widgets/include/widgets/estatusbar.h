#pragma once
#include <QStatusBar>
#include <QWidget>

class EStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    EStatusBar(QWidget *parent = nullptr);
};
