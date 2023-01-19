#ifndef XMLMODELS_H
#define XMLMODELS_H

#include "xmlmodel.h"

/// \brief Class of XML model that stores other XML models as items.
class XmlContainerModel : public XmlModel
{
private:
    QString getModelTagName() const;

public:
    explicit XmlContainerModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
    virtual void create(const QStringList &saved, int *row) override;
    virtual QDomElement toNode(QDomDocument &doc) override;
};

/// \brief Class of XML model that stores data strings as items.
class XmlDataModel : public XmlModel
{
private:
    std::tuple<QString, QString, std::function<void(QDomDocument &, QDomElement &, int &)>> getModelSettings();
    void parseAlarmHighlights(QDomNode &node, int row, int col);

public:
    explicit XmlDataModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
    virtual QDomElement toNode(QDomDocument &doc) override;
};

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
    virtual void getDialogRequest(const int &row) override;
    void update(const QStringList &saved, const int &row) override;
    virtual void create(const QStringList &saved, int *row) override;
};

#endif // XMLMODELS_H
