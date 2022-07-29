#ifndef MODULEDIALOG_H
#define MODULEDIALOG_H

#include <QDialog>

class ModuleDialog : public QDialog
{
    Q_OBJECT
public:
protected:
    QList<std::tuple<QString, QString, bool>> dlgSettings;
    QList<QWidget *> dlgItems;

    void SetupSizePos(int width, int height);
    void SetupUI(const QStringList &inData);

public:
    ModuleDialog(const QStringList &inData, QWidget *parent = nullptr);
};

#endif // MODULEDIALOG_H
