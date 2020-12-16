#include "styleloader.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QKeyEvent>
#include <QMetaEnum>
#include <QSettings>
StyleLoader::StyleLoader(Singleton::token)
{
}

StyleLoader::StyleLoader(Singleton::token, QObject *parent) : QObject(parent)
{
}

void StyleLoader::attach(const QString &filename, QKeySequence key)
{
    m_filename = filename;
    m_key = key;
    qApp->installEventFilter(this);
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
    if (!styleFile().isEmpty())
        m_filename = styleFile();
    QFile file(m_filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open stylesheet file " << m_filename;
        return;
    }
    QString stylesheet = QString::fromUtf8(file.readAll());
    qApp->setStyleSheet(stylesheet);
}

QString StyleLoader::load()
{
    using namespace Style;
    QSharedPointer<QSettings> sets = QSharedPointer<QSettings>(new QSettings("EvelSoft", PROGNAME));
    auto themeEnum = QMetaEnum::fromType<Name>;
    const QString styleName = sets->value("Style").toString();
    if (styleName.isEmpty())
        return defaultStyleFile;
    const int key = themeEnum().keyToValue(styleName.toStdString().c_str());
    return themes.value(Name(key));
}

void StyleLoader::save()
{
    using namespace Style;
    QSharedPointer<QSettings> sets = QSharedPointer<QSettings>(new QSettings("EvelSoft", PROGNAME));

    sets->setValue("Style", styleName());
}

QString StyleLoader::styleFile()
{
    return m_filename;
}

Style::Name StyleLoader::styleNumber()
{
    using namespace Style;
    auto themeEnum = QMetaEnum::fromType<Name>;
    const int key = themeEnum().keyToValue(styleName().toStdString().c_str());
    return Name(key);
}

QString StyleLoader::styleName()
{
    using namespace Style;
    auto themeEnum = QMetaEnum::fromType<Name>;
    const int key = themes.key(styleFile());
    const QString styleName = themeEnum().valueToKey(key);
    return styleName;
}

void StyleLoader::setStyleFile(const QString &styleFile)
{
    m_filename = styleFile;
}
