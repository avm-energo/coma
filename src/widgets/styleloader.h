#pragma once

#include <QKeySequence>
#include <QMap>
#include <QObject>
#include <gen/singleton.h>

constexpr char defaultStyleFile[] = ":/qdarkstyle/light/lightstyle.qss";
constexpr char defaultStyleKey[] = "F5";

namespace Style
{

Q_NAMESPACE
enum Name
{
    Light,
    Dark,
    Drakula,
    Aqua,
    //    Console,
    //    ElegantDark,
    //    ManjaroMix,
    MaterialDark,
    Ubuntu
};
Q_ENUM_NS(Name);

const QMap<Name, QString> themes {
    { Light, defaultStyleFile },                      //
    { Dark, ":/qdarkstyle/dark/darkstyle.qss" },      //
    { Drakula, ":/style/qss-dracula/dracula.css" },   //
    { Aqua, ":/style/QSS/Aqua.qss" },                 //
                                                      //    { Console, ":/style/QSS/ConsoleStyle.qss" },      //
                                                      //    { ElegantDark, ":/style/QSS/ElegantDark.qss" },   //
                                                      //    { ManjaroMix, ":/style/QSS/ManjaroMix.qss" },     //
    { MaterialDark, ":/style/QSS/MaterialDark.qss" }, //
    { Ubuntu, ":/style/QSS/Ubuntu.qss" }              //
};
}

class StyleLoader : public QObject, public Singleton<StyleLoader>
{
    Q_OBJECT

public:
    StyleLoader(Singleton::token, QObject *parent);
    explicit StyleLoader(token);
    void attach(const QString &filename = GetInstance().load(), QKeySequence key = QKeySequence(defaultStyleKey));

    bool eventFilter(QObject *obj, QEvent *event);

    QString styleFile();
    Style::Name styleNumber();
    QString styleName();

    void setStyleFile(const QString &styleFile);
    void setAppStyleSheet();

    QString load();
    void save();

private:
    QString m_filename;
    QKeySequence m_key;
};
