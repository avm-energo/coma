#ifndef S_TQCOMBOBOX_H
#define S_TQCOMBOBOX_H

#include <QComboBox>

class s_tqComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit s_tqComboBox(QWidget *parent = 0);

    void setAData(QVariant adata);
    QVariant getAData();
    void insertItem(int index, const QString &text, const QVariant &userData);

signals:
    void textChanged(const QString &, s_tqComboBox *);
    void textChanged(int, s_tqComboBox *);

public slots:

private:
    QVariant adata;

private slots:
    void changetext(const QString &);
    void changetext(const int);
};

#endif // S_TQCOMBOBOX_H
