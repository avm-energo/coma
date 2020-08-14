#ifndef LINEEDITFIELD_H
#define LINEEDITFIELD_H

#include <QLineEdit>
#include <QPaintEvent>
#include <QWidget>

class LineEditField : public QLineEdit
{
    Q_OBJECT
public:
    explicit LineEditField(QWidget *parent = 0);
    explicit LineEditField(const QString &text, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *e);
};

#endif // LINEEDITFIELD_H
