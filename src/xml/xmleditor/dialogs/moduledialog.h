#ifndef MODULEDIALOG_H
#define MODULEDIALOG_H

#include <QDialog>

class ModuleDialog : public QDialog
{
    Q_OBJECT
private:
    void SetupSizePos(int width, int height);
    void SetupUI();

public:
    ModuleDialog(QWidget *parent = nullptr);
};

#endif // MODULEDIALOG_H
