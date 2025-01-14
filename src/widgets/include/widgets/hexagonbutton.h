#pragma once

#include <QObject>
#include <QPushButton>

class HexagonButton : public QPushButton
{
    Q_OBJECT
public:
    HexagonButton(QWidget *parent = nullptr);
    void setData(const QString &data);
    QString data();

private:
    QString m_data;
};
