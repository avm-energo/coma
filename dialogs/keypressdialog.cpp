#include "keypressdialog.h"

KeyPressDialog::KeyPressDialog(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

void KeyPressDialog::keyPressEvent(QKeyEvent *e)
{
    if ((e->modifiers() == Qt::AltModifier) || (e->modifiers() == Qt::ControlModifier))
        return;
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
    {
        emit Finished(KeyPressString);
        this->close();
    }
    if (e->key() == Qt::Key_Escape)
    {
        emit Finished(QString());
        this->close();
    }
    KeyPressString.append(e->key());
    QDialog::keyPressEvent(e);
}
