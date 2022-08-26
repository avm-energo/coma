#ifndef DIALOGMANAGER_H
#define DIALOGMANAGER_H

#include <QObject>

class DialogManager : QObject
{
    Q_OBJECT
public:
    DialogManager(QObject *parent = nullptr);
};

#endif // DIALOGMANAGER_H
