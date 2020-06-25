#ifndef ETABLEMODEL_H
#define ETABLEMODEL_H

#include <QAbstractTableModel>
#include <QColor>
#include <QFont>
#include <QIcon>
#include <QAbstractItemModel>
#include <QStringList>
#include "etableitem.h"

class ETableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ETableModel(QObject *parent = 0);
    ~ETableModel();

    int result;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role);
    QStringList Headers();
    QVariant data (const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex index(int row, int column, const QModelIndex &index = QModelIndex()) const;
    int rowCount(const QModelIndex &index = QModelIndex()) const;
    int columnCount(const QModelIndex &index = QModelIndex()) const;
    bool insertColumns(int position, int columns, const QModelIndex &index = QModelIndex());
    bool removeColumns(int position, int columns, const QModelIndex &index = QModelIndex());
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
    int getHeaderPosition(QVariant hdrtext, Qt::Orientation orientation, int role); // получение индекса элемента в заголовке, который содержит текст hdrtext
    void addColumn(const QString hdrtext); // добавление новой колонки с текстом в заголовке hdrtext для варианта двух столбцов
    void addRow(); // добавление строки
    void setCellAttr(QModelIndex index, int fcset=0, int icon=-1);
    QString getCellType(int row, int column);
    void setRowAttr(int fcset=0, int icon=-1);
    void ClearModel();
    void fillModel(QVector<QVector<QVariant> > sl);
    QStringList cvalues(int column); // выдать значения по столбцу column в выходной QStringList
    QStringList rvalues(int row); // выдать значения по строке row в выходной QStringList
    void SetRowTextAlignment(int row, int alignment);
    bool isEmpty();

signals:

public slots:

private:
    QList<ETableItem *> maindata;
    QStringList hdr;
    QColor colors[6]; // определение набора цветов шрифта
    QFont fonts[6]; // определение набора шрифтов
    QIcon icons[6]; // определение набора иконок
};

#endif // ETABLEMODEL_H
