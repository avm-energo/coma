#ifndef VALUEMODEL_H
#define VALUEMODEL_H

#include "etablemodel.h"

#include <QObject>

class ValueModel : public ETableModel
{
    Q_OBJECT
public:
    ValueModel(QObject *parent = nullptr);

    void setValueData(const QModelIndex &index, void *valuePtr);
    void updateModel();
    void updateFromModel();

private:
    QMap<QPersistentModelIndex *, void *> m_valuePtrMap;
};

#endif // VALUEMODEL_H
