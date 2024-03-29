#pragma once

#include <QtXml>
#include <xml/xmleditor/models/baseeditormodel.h>

constexpr int FilenameDataRole = 0x0107; ///< Роль для хранения имени файла

/// \brief Class for master model (master workspace, left table view).
class MasterModel final : public BaseEditorModel
{
    Q_OBJECT
private:
    /// \brief Initial function for reading existing configuration files in model.
    void readModulesToModel();
    /// \brief Parsing each file's content in model.
    void parseXmlNode(const QDomNode &node, const QString &filename, const int &row);
    /// \brief Updates a data from a dialog.
    QStringList getNewList(const QStringList &saved);

public:
    explicit MasterModel(QObject *parent = nullptr);

    /// \brief Creates XML DOM node at base of content selected item.
    virtual QDomElement toNode(QDomDocument &doc, const int &row);
    /// \brief Undo changes of opened XML model.
    void undoChanges(const int &row, const bool &openState);

public slots:
    /// \brief Creates XML model in slave workspace where user select item in master model.
    void masterItemSelected(const QModelIndex &index);
    /// \brief Slot for inserting a new item in the model.
    virtual void create(const QStringList &saved, int *row) override;
    /// \brief Slot for updating an item's data in the model.
    virtual void update(const QStringList &saved, const int row) override;
    /// \brief Slot for deleting an exisiting item in the model.
    virtual void remove(const int row) override;

signals:
    void itemSelected(QDomNode &node);
    void createFile(const QStringList &creationData);
    void removeFile(const QString &filename);
};
