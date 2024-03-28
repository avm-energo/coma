#include "styleloader.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QKeyEvent>
#include <QMetaEnum>
#include <QSettings>
#include <settings/user_settings.h>

const QMetaEnum StyleLoader::s_themeEnum = QMetaEnum::fromType<Style::Name>();

StyleLoader::StyleLoader(Singleton::token)
{
}

StyleLoader::StyleLoader(Singleton::token, QObject *parent) : QObject(parent)
{
}

QStringList StyleLoader::availableStyles()
{
    QStringList values;
    for (int i = 0; i < s_themeEnum.keyCount(); i++)
        values.push_back(s_themeEnum.key(i));
    return values;
}

void StyleLoader::attach(const QString &filename, QKeySequence key)
{
    m_filename = filename;
    m_key = key;
    // BUG Остальные eventFilter перестают работать
    // qApp->installEventFilter(this);
    setAppStyleSheet();
}

bool StyleLoader::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (m_key == QKeySequence(keyEvent->key()))
            setAppStyleSheet();
        return true;
    }
    else
        return QObject::eventFilter(obj, event);
}

void StyleLoader::setAppStyleSheet()
{
    QFile file(m_filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Cannot open stylesheet file " << m_filename;
        return;
    }
    QString stylesheet = QString::fromUtf8(file.readAll());
    qApp->setStyleSheet(stylesheet);
}

QString StyleLoader::load()
{
    const QString styleName = Settings::UserSettings::GetInstance().get<Settings::Theme>();
    if (styleName.isEmpty())
        return defaultStyleFile;
    const int key = s_themeEnum.keyToValue(styleName.toStdString().c_str());
    return Style::themes.value(Style::Name(key));
}

void StyleLoader::save()
{
    Settings::UserSettings::GetInstance().set<Settings::Theme>(styleName());
}

void StyleLoader::setStyle(const QString &styleName)
{
    auto key = Style::Name(s_themeEnum.keyToValue(styleName.toStdString().c_str()));
    m_filename = Style::themes.value(key);
}

Style::Name StyleLoader::styleNumber()
{
    const int key = s_themeEnum.keyToValue(styleName().toStdString().c_str());
    return Style::Name(key);
}

QString StyleLoader::styleName()
{
    const int key = Style::themes.key(m_filename);
    const QString styleName = s_themeEnum.valueToKey(key);
    return styleName;
}

void StyleLoader::setStyleFile(const QString &styleFile)
{
    m_filename = styleFile;
}
