#ifndef MODULEDIALOG_H
#define MODULEDIALOG_H

#include "xmleditor/dialogs/xmldialog.h"

#include <QDialog>

class ModuleDialog : public QDialog
{
    Q_OBJECT
protected:
    bool isChanged;
    QList<std::tuple<QString, QString, bool>> dlgSettings;
    QList<QWidget *> dlgItems;
    int mRow;
    QString mTitle;

    void SetupSizePos(int &width, int &height);

public:
    ModuleDialog(QWidget *parent = nullptr);
    void SetData(const int row = createId);
    void SetupUI(int width, int height, const QString &title);

signals:
    void ModelDataRequest(const int &row);
    void CreateData(const QStringList &saved, int *rowPtr);
    void UpdateData(const QStringList &saved, const int &row);

public slots:
    void ModelDataResponse(const QStringList &response);
    void DataChanged();
    void DataChanged(const QString &str);
    void SaveData();
};

#endif // MODULEDIALOG_H
