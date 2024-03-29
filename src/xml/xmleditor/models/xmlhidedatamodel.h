#pragma once

#include "xmlmodel.h"

constexpr int SGroupDataRole = 0x0106; ///< Role for hiding data SGroupHideData.

/// \brief Structure, that stores a hiding data.
struct SGroupHideData
{
    int count = 1;
    QString tooltip = "";
    QString view = "float";
    QStringList array = {};
};
Q_DECLARE_METATYPE(SGroupHideData);

/// \brief Class for storing hiding data.
class XmlHideDataModel : public XmlModel
{
private:
    SGroupHideData parseHideData(QDomNode &node);
    SGroupHideData convertHideData(const QStringList &input);

public:
    explicit XmlHideDataModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
    virtual QDomElement toNode(QDomDocument &doc) override;

public slots:
    virtual void getDialogRequest(const int row) override;
    void update(const QStringList &saved, const int row) override;
    virtual void create(const QStringList &saved, int *row) override;
};
