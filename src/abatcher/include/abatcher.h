#pragma once

#include <QMainWindow>
#include <QCheckBox>

class Abatcher : public QMainWindow
{
    Q_OBJECT

public:
    explicit Abatcher(QWidget *parent = nullptr);
    virtual ~Abatcher();

    void run();

private:
    void setupUI();
    QCheckBox* createDryCheckBox();
};