/* Keypressdialog
 * It's useful for making a non-echo input of passwords and similar security
 * things Start it as a usual QDialog and connect its signal Finished(QString)
 * to the corresponding slot in the prog. Dialog is auto-closing when user press
 * Enter key or Esc key. Signal Cancelled() is emitted when user press Esc key
 */

#ifndef KEYPRESSDIALOG_H
#define KEYPRESSDIALOG_H

#include "../gen/error.h"

#include <QCloseEvent>
#include <QDialog>
#include <QKeyEvent>

class KeyPressDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KeyPressDialog(QWidget *parent = nullptr); // PswPhrase - фраза, которую надо написать пользователю
    bool CheckPassword(const QString &psw);

signals:
    //    void Finished(QString str);
    void Cancelled();
    void PasswordChecked();

public slots:
    //    void PasswordCheck(QString psw);

private:
    //    bool m_pswValid;
    QString m_pswEntered;

    void SetupUI();

protected:
    void keyPressEvent(QKeyEvent *e);
    void closeEvent(QCloseEvent *e);
};

#endif // KEYPRESSDIALOG_H
