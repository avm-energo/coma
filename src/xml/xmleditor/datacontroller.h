#pragma once

#include "models/xmlmodel.h"

#include <QObject>

class DataController : public QObject
{
    Q_OBJECT
private:
    bool isModuleChanged;
    int changedRow;

    QString getFilePath(const QString &filename);

public:
    explicit DataController(QObject *parent = nullptr);
    void resetOrSaved();
    bool getModuleState() const;
    int getRow() const;
    void setRow(const int &row);

public slots:
    void createFile(const QStringList &creationData);
    void renameFile(const QString &oldName, const QString &newName);
    void removeFile(const QString &filename);
    void configChanged();

signals:
    void highlightModified();
};
