#pragma once

#include <gen/singleton.h>
#include <oscillograms/trendview/trendviewmodel.h>

#include <QAction>
#include <QWidget>

class ComaHelper : public QObject, public Singleton<ComaHelper>
{
    Q_OBJECT
public:
    explicit ComaHelper(QWidget *parent);
    void initAppSettings(const QString &appName, const QString &orgName, const QString &version);
    void parseCommandLine();
    void loadOsc(const QString &filename);
    void loadOsc(TrendViewModel *model);
    void loadSwj(const QString &filename);
    void loadJournal(const QString &filename);

signals:

public slots:
    static void convertPixmap(size_t size, QAction *jourAct);
    void loadOsc();
    void loadSwj();
    void loadJournal();

private:
    QWidget *m_parent;
};
