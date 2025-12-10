#pragma once

#include <QGroupBox>

class EGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    explicit EGroupBox(QWidget *parent = 0);
    explicit EGroupBox(const QString &caption, QWidget *parent = 0);

    //    void insertItem(int index, const QString &text, const QVariant &userData);

signals:

public slots:

private:
private slots:
};
