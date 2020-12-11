#include "keypressdialog.h"

#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "../widgets/wd_func.h"

#include <QEventLoop>
#include <QMessageBox>
#include <QVBoxLayout>

KeyPressDialog::KeyPressDialog(QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    SetupUI();
    SetPhrase("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
}

bool KeyPressDialog::CheckPassword(const QString &psw)
{
    //    m_pswValid = false;
    StdFunc::clearCancel();
    QEventLoop PasswordLoop;
    connect(this, &KeyPressDialog::PasswordChecked, &PasswordLoop, &QEventLoop::quit);
    show();
    PasswordLoop.exec();
    if (StdFunc::isCancelled())
    {
        qCritical("Отмена ввода пароля");
        return false;
    }
    if (m_pswEntered != psw)
    {
        qCritical("Пароль введён неверно");
        QMessageBox::critical(this, "Неправильно", "Пароль введён неверно", QMessageBox::Ok);
        return false;
    }
    return true;
}

void KeyPressDialog::SetupUI()
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(
        WDFunc::NewLBL(this, "Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc", "", "pswlbl"));
    vlyout->addWidget(WDFunc::NewPswLE(this, "pswle", QLineEdit::Password));
    setLayout(vlyout);
}

void KeyPressDialog::SetPhrase(const QString &Phrase)
{
    WDFunc::LE_write_data(this, Phrase, "pswlbl");
}

void KeyPressDialog::keyPressEvent(QKeyEvent *e)
{
    QString str;
    if ((e->modifiers() == Qt::AltModifier) || (e->modifiers() == Qt::ControlModifier))
    {
        ERMSG("Ошибка при обработке пароля");
        return;
    }
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
    {
        if ((m_pswEntered = WDFunc::LEData(this, "pswle")) != "")
            emit PasswordChecked();
    }
    if (e->key() == Qt::Key_Escape)
    {
        StdFunc::cancel();
        emit PasswordChecked();
    }
    QDialog::keyPressEvent(e);
}

void KeyPressDialog::closeEvent(QCloseEvent *e)
{
    QString str;
    m_pswEntered = WDFunc::LEData(this, "pswle");
    emit PasswordChecked();
    //    else
    //        emit Finished(QString());
    e->accept();
}
