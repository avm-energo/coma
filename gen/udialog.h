#ifndef UDIALOG_H
#define UDIALOG_H

// simple dialog class to populate from
// has virtual methods setUpdatesDisabled & setUpdatesEnabled

#include "../interfaces/baseinterface.h"

#include <QDialog>

class UDialog : public QDialog
{
    Q_OBJECT
public:
    bool m_updatesEnabled;

    UDialog(QWidget *parent = nullptr);

    virtual void setUpdatesEnabled();
    virtual void setUpdatesDisabled();
    virtual void setConnections() = 0;
    const QString getCaption();
    void setCaption(const QString &caption);
    void setInterface(BaseInterface *iface);
    BaseInterface *iface();

public slots:
    virtual void update();

private:
    QString m_caption;
    BaseInterface *m_iface;
};

#endif // UDIALOG_H
