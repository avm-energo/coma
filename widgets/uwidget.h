#ifndef UWIDGET_H
#define UWIDGET_H

// simple widget class to populate from
// has virtual methods setUpdatesDisabled & setUpdatesEnabled

#include "../interfaces/baseinterface.h"

#include <QWidget>

class UWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UWidget(QWidget *parent = nullptr);

    void setUpdatesEnabled();
    void setUpdatesDisabled();
    virtual void setConnections() = 0;
    const QString getCaption();
    void setCaption(const QString &caption);
    void setInterface(BaseInterface *iface);
    BaseInterface *iface();
signals:

public slots:
    virtual void update();

private:
    bool m_updatesEnabled;
    QString m_caption;
    BaseInterface *m_iface;
};

#endif // UWIDGET_H
