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

    void SetupUI(QSize&);
    QVBoxLayout *GetWorkspace(WorkspaceType);

public:
    explicit ModulesEditor(QWidget *parent = nullptr);



signals:

public slots:
    void Close();

};

#endif // MODULESEDITOR_H
