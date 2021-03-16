#ifndef TUNE84CHECK_H
#define TUNE84CHECK_H

#include "../abstracttunedialog.h"

#include <QShowEvent>
#include <QWidget>

class Tune84Check : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune84Check(int tuneStep, QWidget *parent = nullptr);

private:
    void setMessages() override;
    void setTuneFunctions() override;
    Error::Msg showScheme();
    Error::Msg check();
private slots:

protected:
    void showEvent(QShowEvent *e) override;
};

#endif // TUNE84CHECK_H
