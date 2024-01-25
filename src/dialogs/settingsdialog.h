#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QMap>
#include <settings/app_settings.h>

class QStackedWidget;
class QListWidget;
class QVBoxLayout;

class SettingsDialog : public QDialog
{
    Q_OBJECT
private:
    Settings::ApplicationSettings &m_settings;
    QStackedWidget *m_workspace;
    QListWidget *m_sidebar;

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private:
    void setupUI();
    QVBoxLayout *createTabWidget(const QString &tabName);

    void setupGeneralTab();
    void setupConnectionTab();
    void setupTuneTab();
    void fill();

private slots:
    void acceptSettings();
    void themeChanged(const QString &newTheme);

signals:
    void disableAlarmUpdate();
};

#endif // SETTINGSDIALOG_H
