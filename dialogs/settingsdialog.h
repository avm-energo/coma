#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QMap>
class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    enum Style : quint8
    {
        Dark,
        Drakula,
        //        Amoled,
        Aqua,
        Console,
        ElegantDark,
        ManjaroMix,
        MaterialDark,
        Ubuntu
    };
    Q_ENUM(Style);
    explicit SettingsDialog(QWidget *parent = 0);

signals:

public slots:

private:
    void SetupUI();
    void Fill();

    QMap<Style, QString> themes {
        { Dark, ":qdarkstyle/style.qss" },                //
        { Drakula, ":/style/qss-dracula/dracula.css" },   //
        /* { Amoled, ":/style/QSS/AMOLED.qss" }, */       //
        { Aqua, ":/style/QSS/Aqua.qss" },                 //
        { Console, ":/style/QSS/ConsoleStyle.qss" },      //
        { ElegantDark, ":/style/QSS/ElegantDark.qss" },   //
        { ManjaroMix, ":/style/QSS/ManjaroMix.qss" },     //
        { MaterialDark, ":/style/QSS/MaterialDark.qss" }, //
        { Ubuntu, ":/style/QSS/Ubuntu.qss" }              //
    };

private slots:
    void AcceptSettings();
};

#endif // SETTINGSDIALOG_H
