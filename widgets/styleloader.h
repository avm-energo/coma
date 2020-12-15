#pragma once

#include <QKeySequence>
#include <QObject>
class StyleLoader : public QObject
{
    Q_OBJECT
public:
    static void attach(const QString &filename = defaultStyleFile(), QKeySequence key = QKeySequence("F5"));

    bool eventFilter(QObject *obj, QEvent *event);

    static QString styleFile();
    static void setStyleFile(const QString &styleFile);

private:
    StyleLoader(QObject *parent, const QString &filename, const QKeySequence &key);
    void setAppStyleSheet();
    static QString defaultStyleFile();
    static QString m_styleFile;
    QString m_filename;
    QKeySequence m_key;
};
