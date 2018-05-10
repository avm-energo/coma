#include <QVBoxLayout>
#include "keypressdialog.h"
#include "../gen/publicclass.h"
#include "../widgets/wd_func.h"

KeyPressDialog::KeyPressDialog(const QString &PswPhrase, QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    SetPhrase(PswPhrase);
}

void KeyPressDialog::SetupUI()
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL(this, "Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc", "", "pswlbl"));
    vlyout->addWidget(WDFunc::NewPswLE(this, "pswle", QLineEdit::Password));
    setLayout(vlyout);
}

void KeyPressDialog::SetPhrase(const QString &Phrase)
{
    WDFunc::SetLEData(this, "pswlbl", Phrase);
}

void KeyPressDialog::keyPressEvent(QKeyEvent *e)
{
    QString str;
    if ((e->modifiers() == Qt::AltModifier) || (e->modifiers() == Qt::ControlModifier))
        return;
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
    {
        if (WDFunc::LEData(this, "pswle", str))
            emit Finished(str);
        this->close();
    }
    if (e->key() == Qt::Key_Escape)
    {
        emit Finished(QString());
        pc.Cancelled = true;
        this->close();
    }
    QDialog::keyPressEvent(e);
}
