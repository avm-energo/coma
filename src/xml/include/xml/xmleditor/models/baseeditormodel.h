#pragma once

#include <QStandardItemModel>
#include <QtXml>

/// \brief Enumeration for saving type of submodule.
enum class ModelType : quint16
{
    None = 0,
    Master,
    Resources,
    Signals,
    SectionTabs,
    Sections,
    Section,
    SGroup,
    Alarms,
    AlarmStateAll,
    AlarmsCrit,
    AlarmsWarn,
    AlarmsInfo,
    Journals,
    WorkJours,
    MeasJours,
    Modbus,
    Protocom,
    IEC60870,
    Config,
    Hidden,
    HiddenTab,
    BsiExt,
};

/// \brief Base class for models that used in XML editor.
class BaseEditorModel : public QStandardItemModel
{
    Q_OBJECT
protected:
    ModelType m_type;

    /// \brief Creates an attribute at base of input data.
    /// \param doc - XML document in that creates data.
    /// \param elem - XML DOM node, child of doc. New attribute creates for this node.
    /// \param attrName - a name of creating attribute.
    /// \param attrVar - QVariant with string value of creating attribute.
    void setAttribute(QDomDocument &doc, QDomElement &elem, const QString &attrName, const QVariant &attrVar);

    /// \brief Creates QDomNode at base of input QDomDocument.
    QDomElement makeElement(QDomDocument &doc, const QString &elemName);

    /// \brief Creates a QDomNode at base of input data.
    /// \param doc - XML document in that creates data.
    /// \param parent - XML DOM node, parent of creating node.
    /// \param elemName - name of creating node.
    /// \param data - string data for the new node.
    void makeElement(QDomDocument &doc, QDomElement &parent, const QString &elemName, const QString &data);

    /// \brief Creates a QDomNode at base of input data.
    /// \param doc - XML document in that creates data.
    /// \param parent - XML DOM node, parent of creating node.
    /// \param elemName - name of creating node.
    /// \param data - QVariant with string data for the new node.
    void makeElement(QDomDocument &doc, QDomElement &parent, const QString &elemName, const QVariant &data);

public:
    explicit BaseEditorModel(int rows, int cols, ModelType type, QObject *parent = nullptr);

    /// \brief Sets horizontal header labels of model from an input string list.
    virtual void setHorizontalHeaderLabels(const QStringList &labels);
    /// \brief Returns a model type.
    ModelType getModelType() const;

signals:
    /// \details This signal is emitted when dialog sends a request for the model's item data.
    void sendDialogResponse(const QStringList &response);
    /// \details This signal is emitted when model's data changes.
    void modelChanged();

public slots:
    /// \brief Slot for receiving a request from dialog and emits signal with response.
    virtual void getDialogRequest(const int row);
    /// \brief Slot for inserting a new item in the model.
    virtual void create(const QStringList &saved, int *row);
    /// \brief Slot for updating an item's data in the model.
    virtual void update(const QStringList &saved, const int row);
    /// \brief Slot for deleting an exisiting item in the model.
    virtual void remove(const int row);
};
