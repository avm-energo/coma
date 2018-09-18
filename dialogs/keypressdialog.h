/* Keypressdialog
 * It's useful for making a non-echo input of passwords and similar security things
 * Start it as a usual QDialog and connect its signal Finished(QString) to the
 * corresponding slot in the prog. Dialog is auto-closing when user press Enter key
 * or Esc key. Signal Cancelled() is emitted when user press Esc key
 */

#ifndef KEYPRESSDIALOG_H
#define KEYPRESSDIALOG_H

#include <QDialog>
#include <QKeyEvent>

class KeyPressDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KeyPressDialog(const QString &PswPhrase, QWidget *parent = nullptr); // PswPhrase - фраза, которую надо написать пользователю

signals:
    void Finished(QString str);
    void Cancelled();

public slots:

private:
    void SetupUI();
    void SetPhrase(const QString &Phrase);

protected:
    void keyPressEvent(QKeyEvent *e);
};

#endif // KEYPRESSDIALOG_H
