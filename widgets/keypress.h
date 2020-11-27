#ifndef KEYPRESS_H
#define KEYPRESS_H

#include <QKeyEvent>
#include <QWidget>

class Keypress : public QWidget
{
    Q_OBJECT
public:
    explicit Keypress(QWidget *parent = nullptr);

signals:
    void Finished();

public slots:

protected:
    void keyPressEvent();
};

#endif // KEYPRESS_H
