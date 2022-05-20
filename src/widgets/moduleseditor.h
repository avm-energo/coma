#ifndef MODULESEDITOR_H
#define MODULESEDITOR_H

#include <QWidget>
#include <QHBoxLayout>

class ModulesEditor : public QWidget
{
    Q_OBJECT

private:
    enum WorkspaceType
    {
        Master = 1,
        Slave = 2
    };

    QVBoxLayout *master;
    QVBoxLayout *slave;

    void SetupUI();
    QVBoxLayout *GetWorkspace(WorkspaceType);

public:
    explicit ModulesEditor(QWidget *parent = nullptr);



signals:

private slots:
    void Close();

};

#endif // MODULESEDITOR_H
