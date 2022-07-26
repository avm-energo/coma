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
    QHash<int, QVariant> hideData;

    SGroupHideData parseHideData(QDomNode &node);

public:
    explicit XmlSGroupModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual QVariant data(const QModelIndex &index, int nRole = Qt::UserRole + 1) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &val, int nRole = Qt::UserRole + 1) override;
    virtual void parseNode(QDomNode &node, int &row) override;
};

#endif // XMLSGROUPMODEL_H
