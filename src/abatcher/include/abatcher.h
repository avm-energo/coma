#pragma once

#include <QCheckBox>
#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QTimer>

class Abatcher : public QMainWindow
{
    Q_OBJECT

public:
    explicit Abatcher(QWidget *parent = nullptr);
    virtual ~Abatcher();

    void run();

private:
    QStackedWidget *m_widgets = nullptr;

    QCheckBox *m_isDry = nullptr;
    QPushButton *m_startButton = nullptr;

    QTableView *m_usbCons = nullptr;
    QStandardItemModel *m_usbModel = nullptr;
    QTimer *m_updateTicker = nullptr;

    void setupUI();
    void createDryCheckBox();
    void createUSBConnectionsWindow();
    void updateUSBConnectionWindow();
    void createStartPushButton();
};