#pragma once

#include <QPushButton>

class EPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit EPushButton(QWidget *parent = 0);
    explicit EPushButton(const QString &caption, QWidget *parent = 0);

    //    void insertItem(int index, const QString &text, const QVariant &userData);

signals:

public slots:

private:
private slots:
};
