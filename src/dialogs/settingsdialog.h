#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QMap>
class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);

private:
    void SetupUI();
    void Fill();

private slots:
    void AcceptSettings();
signals:
    void disableAlarmUpdate();
};

#endif // SETTINGSDIALOG_H
