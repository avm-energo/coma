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
};

/// \brief Base class for models that used in XML editor.
class BaseEditorModel : public QStandardItemModel
{
    Q_OBJECT
protected:
    ModelType mType;
    void setAttribute(QDomDocument &doc, QDomElement &elem, const QString &attrName, const QVariant &attrVar);
    QDomElement makeElement(QDomDocument &doc, const QString &elemName);
    void makeElement(QDomDocument &doc, QDomElement &parent, const QString &elemName, const QString &data);
    void makeElement(QDomDocument &doc, QDomElement &parent, const QString &elemName, const QVariant &data);

public:
    explicit BaseEditorModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void setHorizontalHeaderLabels(const QStringList &labels);
    ModelType getModelType() const;

signals:
    /// \details This signal is emitted when dialog sends a request for the model's item data.
    void sendDialogResponse(const QStringList &response);
    /// \details This signal is emitted when model's data changes.
    void modelChanged();

public slots:
    virtual void getDialogRequest(const int row);
    virtual void create(const QStringList &saved, int *row);
    virtual void update(const QStringList &saved, const int row);
    virtual void remove(const int row);
};
