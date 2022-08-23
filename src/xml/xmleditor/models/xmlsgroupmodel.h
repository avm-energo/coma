#ifndef XMLSGROUPMODEL_H
#define XMLSGROUPMODEL_H

#include "xmlmodel.h"

constexpr int SGroupDataRole = 0x0106; ///< Роль для скрытых данных SGroup модели

/// \brief Структура, хранящая скрытые данные
struct SGroupHideData
{
    int count = 1;
    QString tooltip = "";
    QStringList array = {};
};
Q_DECLARE_METATYPE(SGroupHideData);

class XmlSGroupModel : public XmlModel
{
private:
    SGroupHideData parseHideData(QDomNode &node);
    SGroupHideData convertHideData(const QStringList &input);

public:
    explicit XmlSGroupModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
    virtual QDomElement *toNode(QDomDocument &doc) override;

public slots:
    virtual void getDialogRequest(const int &row) override;
    void update(const QStringList &saved, const int &row) override;
};

#endif // XMLSGROUPMODEL_H
