#ifndef KEYPRESS_H
#define KEYPRESS_H

#include <QWidget>
#include <QKeyEvent>

class Keypress : public QWidget
{
    Q_OBJECT
public:
    explicit Keypress(QWidget *parent = 0);

signals:
    void Finished();

public slots:

protected:
    void keyPressEvent();
};

#endif // KEYPRESS_H
