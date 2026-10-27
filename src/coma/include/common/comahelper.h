#pragma once

#include <journals/journalviewer.h>
#include <libavm-gen/messagehandler.h>

#include <QAction>
#include <QWidget>

class ComaHelper : public QObject
{
    Q_OBJECT
public:
    explicit ComaHelper();
    void initAppSettings();
    void parseCommandLine();
    void loadSwj(const QString &filename);
    void loadJournal(const QString &filename, journals::JournalType type);
    void loadOsc();

signals:

public slots:
    static void convertPixmap(size_t size, QAction *jourAct);
    void loadSwj();
    void loadJournal();

private:
};
