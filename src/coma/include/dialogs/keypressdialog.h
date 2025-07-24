/* Keypressdialog
 * It's useful for making a non-echo input of passwords and similar security
 * things Start it as a usual QDialog and connect its signal Finished(QString)
 * to the corresponding slot in the prog. Dialog is auto-closing when user press
 * Enter key or Esc key. Signal Cancelled() is emitted when user press Esc key
 */

#pragma once

#include <QCloseEvent>
#include <QDialog>
#include <QKeyEvent>
#include <gen/error.h>

class KeyPressDialog final : public QDialog
{
    Q_OBJECT
public:
    explicit KeyPressDialog(QWidget *parent = nullptr);
    bool CheckPassword(const QString &password);

signals:
    void finished();

private:
    QString m_password;

    void SetupUI();

protected:
    void keyPressEvent(QKeyEvent *e);
    void closeEvent(QCloseEvent *e);
};
