#pragma once

#include <QObject>
#include <QVariant>
#include <QtXml/QDomDocument>

class MasterModel;
class XmlModel;

/// \brief Контроллер данных в архитектуре MVC.
class DataController : public QObject
{
    Q_OBJECT
private:
    bool isModuleChanged;
    int changedRow;

    QString getFilePath(const QString &filename);
    void replaceChild(QDomDocument &doc, QDomElement &parent, const QString &tag, const QVariant &data);
    void replaceAttrs(QDomDocument &doc, QDomElement &node, const QStringList attrs, const QList<QVariant> &vals);
    void writeToFile(const QDomDocument &doc, const QString &oldName, const QString &newName);

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
    void saveFile(MasterModel *masterModel);

signals:
    void highlightModified();
};
