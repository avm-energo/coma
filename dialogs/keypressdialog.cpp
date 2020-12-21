#include "keypressdialog.h"

#include "../gen/error.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QEventLoop>
#include <QMessageBox>
#include <QVBoxLayout>
KeyPressDialog::KeyPressDialog(QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    SetupUI();
}

bool KeyPressDialog::CheckPassword(const QString &password)
{
    QEventLoop PasswordLoop;
    connect(this, &KeyPressDialog::finished, &PasswordLoop, &QEventLoop::quit);
    show();
    PasswordLoop.exec();
    close();
    if (m_password.isEmpty() || m_password.isNull())
    {
        qCritical("Отмена ввода пароля");
        return false;
    }

    if (m_password != password)
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
        WDFunc::NewLBL2(this, "Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc", "pswlbl"));
    vlyout->addWidget(WDFunc::NewPswLE2(this, "pswle", QLineEdit::Password));
    setLayout(vlyout);
}

void KeyPressDialog::keyPressEvent(QKeyEvent *e)
{
    QString str;
    if ((e->modifiers() == Qt::AltModifier) || (e->modifiers() == Qt::ControlModifier))
    {
        qCritical("Ошибка при обработке пароля");
        return;
    }

    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
    {
        const QString password = WDFunc::LEData(this, "pswle");
        if (!password.isEmpty())
            m_password = password;
        emit finished();
    }
    if (e->key() == Qt::Key_Escape)
    {
        emit finished();
    }
    QDialog::keyPressEvent(e);
}

void KeyPressDialog::closeEvent(QCloseEvent *e)
{
    emit finished();
    e->accept();
}
