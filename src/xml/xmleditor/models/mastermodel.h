#pragma once

#include "../../xmltags.h"
#include "baseeditormodel.h"

#include <QtXml>

constexpr int FilenameDataRole = 0x0107; ///< Роль для хранения имени файла

/// \brief Class for master model (master workspace, left table view).
class MasterModel : public BaseEditorModel
{
    Q_OBJECT
private:
    void readModulesToModel();
    void parseXmlNode(const QDomNode &node, const QString &filename, const int &row);
    QStringList getNewList(const QStringList &saved);

public:
    MasterModel(QObject *parent = nullptr);
    virtual QDomElement toNode(QDomDocument &doc, const int &row);
    void undoChanges(const int &row, const bool &openState);

public slots:
    void masterItemSelected(const QModelIndex &index);
    virtual void create(const QStringList &saved, int *row) override;
    virtual void update(const QStringList &saved, const int &row) override;
    virtual void remove(const int &row) override;

signals:
    void itemSelected(QDomNode &node);
    void createFile(const QStringList &creationData);
    void removeFile(const QString &filename);
};
