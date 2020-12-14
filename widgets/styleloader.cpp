#include "styleloader.h"

#include "../gen/stdfunc.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QKeyEvent>
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
    return StdFunc::GetSystemHomeDir() + "style.qss";
    // QIcon(":/icons/tnread.svg")
    // return QApplication::applicationDirPath() + "/style.qss";
}

StyleLoader::StyleLoader(QObject *parent, const QString &filename, const QKeySequence &key)
    : QObject(parent), m_filename(filename), m_key(key)
{
}
