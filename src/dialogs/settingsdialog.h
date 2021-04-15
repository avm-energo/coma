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

namespace settings
{
constexpr char logKey[] = "WriteLog";
constexpr char logWidget[] = "writelogchb";
constexpr char tuneCountKey[] = "TuneRequestCount";
constexpr char tuneCountWidget[] = "reqcount";
constexpr char timezoneKey[] = "Timezone";
constexpr char timezoneWidget[] = "timezoneCB";
}

#endif // SETTINGSDIALOG_H
