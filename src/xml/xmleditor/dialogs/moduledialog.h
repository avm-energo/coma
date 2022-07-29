#ifndef MODULEDIALOG_H
#define MODULEDIALOG_H

#include <QDialog>

class ModuleDialog : public QDialog
{
    Q_OBJECT
public:
    enum Behaviour : quint16
    {
        Create = 0,
        Edit = 1
    };

private:
    void SetupSizePos(int width, int height);
    void SetupUI(Behaviour &behav);

public:
    ModuleDialog(Behaviour behav, QWidget *parent = nullptr);
};

#endif // MODULEDIALOG_H
