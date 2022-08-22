#pragma once

#include "models/mastermodel.h"
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
    void configChanged();
    void createFile(const QStringList &creationData);
    void renameFile(const QString &oldName, const QString &newName);
    void removeFile(const QString &filename);
    void saveFile(MasterModel *masterModel, XmlModel *slaveModel);

signals:
    void highlightModified();
};
