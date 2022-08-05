#pragma once

#include <QStandardItemModel>
#include <QtXml>

namespace tags
{
/// \brief Enumeration for saving type of submodule.
enum NodeTypes : quint16
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
    AlarmsItem,
    Journals,
    WorkJours,
    MeasJours,
    Modbus,
    Protocom,
    IEC60870,
    Config
};
}
using ModelType = tags::NodeTypes;

class IEditorModel : public QStandardItemModel
{
    Q_OBJECT
protected:
    ModelType mType;
    void setAttribute(QDomDocument &doc, const QString &attrName, const QVariant &attrVar);

public:
    explicit IEditorModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void setHorizontalHeaderLabels(const QStringList &labels);
    ModelType getModelType() const;
    virtual QDomElement *toNode();

signals:
    void sendDialogResponse(const QStringList &response);
    void modelChanged();

public slots:
    virtual void getDialogRequest(const int &row);
    virtual void create(const QStringList &saved, int *row);
    virtual void update(const QStringList &saved, const int &row);
    virtual void remove(const int &row);
};
