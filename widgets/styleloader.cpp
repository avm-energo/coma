#include "styleloader.h"

#include "../gen/stdfunc.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QKeyEvent>

QString StyleLoader::m_styleFile {};

void StyleLoader::attach(const QString &filename, QKeySequence key)
{
    StyleLoader *loader = new StyleLoader(qApp, filename, key);
    qApp->installEventFilter(loader);
    loader->setAppStyleSheet();
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

QString StyleLoader::defaultStyleFile()
{
    return ":qdarkstyle/style.qss";
    /*StdFunc::GetSystemHomeDir()*/
    // QIcon(":/icons/tnread.svg")
    // return QApplication::applicationDirPath() + "/style.qss";
}

QString StyleLoader::styleFile()
{
    return m_styleFile;
}

void StyleLoader::setStyleFile(const QString &styleFile)
{
    m_styleFile = styleFile;
}

StyleLoader::StyleLoader(QObject *parent, const QString &filename, const QKeySequence &key)
    : QObject(parent), m_filename(filename), m_key(key)
{
}
