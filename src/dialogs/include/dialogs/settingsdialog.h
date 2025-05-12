#pragma once

#include <QDialog>
#include <QMap>

class QStackedWidget;
class QListWidget;
class QVBoxLayout;
class QTabWidget;

class SettingsDialog final : public QDialog
{
    Q_OBJECT
private:
    QStackedWidget *m_workspace;
    QListWidget *m_sidebar;

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog() noexcept;

private:
    void setupUI();
    QVBoxLayout *createWorkspaceLayout(const QString &tabName);
    QTabWidget *createTabWidget(QVBoxLayout *layout);
    QVBoxLayout *createTabLayout(QTabWidget *destination, const QString &tabName);

    void setupGeneralTab() noexcept;
    void setupConnectionTab() noexcept;
    void setupTuneTab() noexcept;
    void fill();

private slots:
    void acceptSettings();
    void resetPassword();
    void updatePassword(const QString &newPassword);
    void themeChanged(const QString &newTheme);

signals:
    void alarmOperationUpdate(const bool status);
    void alarmIntervalUpdate(const int interval);
};
