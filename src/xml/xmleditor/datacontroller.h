#pragma once

#include <QObject>

class DataController : public QObject
{
    Q_OBJECT
private:
    QString getFilePath(const QString &filename);

public:
    explicit DataController(QObject *parent = nullptr);

public slots:
    void createFile(const QStringList &creationData);
    void renameFile(const QString &oldName, const QString &newName);
    void removeFile(const QString &filename);
};
