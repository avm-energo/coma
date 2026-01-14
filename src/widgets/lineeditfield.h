#ifndef LINEEDITFIELD_H
#define LINEEDITFIELD_H

#include <QWidget>
#include <QLineEdit>
#include <QPaintEvent>

class LineEditField : public QLineEdit
{
    Q_OBJECT
public:
    explicit LineEditField(QWidget *parent=0);
    explicit LineEditField(const QString &text, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *e);
};

#endif // LINEEDITFIELD_H
