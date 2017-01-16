#ifndef S_TQCOMBOBOX_H
#define S_TQCOMBOBOX_H

#include <QComboBox>

class s_tqComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit s_tqComboBox(QWidget *parent = 0);

    void insertItem(int index, const QString &text, const QVariant &userData);

signals:

public slots:

private:

private slots:
};

#endif // S_TQCOMBOBOX_H
