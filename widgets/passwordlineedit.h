#ifndef PASSWORDLINEEDIT_H
#define PASSWORDLINEEDIT_H

#include <QLineEdit>

class PasswordLineEdit : public QWidget
{
    Q_OBJECT

public:
    explicit PasswordLineEdit(QWidget *parent = Q_NULLPTR);
    explicit PasswordLineEdit(QLineEdit::EchoMode echostyle = QLineEdit::Password, QWidget *parent = Q_NULLPTR);

private slots:
    void PswLEToggleEchoMode();
};

#endif // PASSWORDLINEEDIT_H
