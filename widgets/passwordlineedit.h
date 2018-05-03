#ifndef PASSWORDLINEEDIT_H
#define PASSWORDLINEEDIT_H

#include <QLineEdit>

class PasswordLineEdit : public QWidget
{
    Q_OBJECT

public:
    PasswordLineEdit(QWidget *parent = Q_NULLPTR, QLineEdit::EchoMode echostyle = QLineEdit::Password);

private slots:
    void PswLEToggleEchoMode();
};

#endif // PASSWORDLINEEDIT_H
