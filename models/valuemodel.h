#ifndef VALUEMODEL_H
#define VALUEMODEL_H

#include "etablemodel.h"
#include "../gen/datablock.h"

#include <QObject>

class ValueModel : public ETableModel
{
    Q_OBJECT
public:
    enum TableModelRoles
    {
        DataFormatRole = Qt::UserRole,
        AdditionalDataRole = Qt::UserRole + 1
    };

    ValueModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    void setValueData(const QModelIndex &index, void *valuePtr);
    void updateModel();
    void updateFromModel();

private:
    QMap<QPersistentModelIndex *, void *> m_valuePtrMap;
};

#endif // VALUEMODEL_H
