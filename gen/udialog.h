#ifndef UDIALOG_H
#define UDIALOG_H

// simple dialog class to populate from
// has virtual methods setUpdatesDisabled & setUpdatesEnabled

#include <QDialog>

class UDialog : public QDialog
{
    Q_OBJECT
public:
    bool m_updatesEnabled;

    UDialog(QWidget *parent = nullptr);

    virtual void setUpdatesEnabled();
    virtual void setUpdatesDisabled();
    const QString getCaption();
    void setCaption(const QString &caption);

public slots:
    virtual void update();

private:
    QString m_caption;
};

#endif // UDIALOG_H
