#include "moduledialog.h"

#include "../../../widgets/wd_func.h"

#include <QGuiApplication>
#include <QScreen>

ModuleDialog::ModuleDialog(QWidget *parent) : QDialog(parent)
{
    SetupUI();
    this->exec();
}

void ModuleDialog::SetupUI()
{
    ;
    ;
}

void ModuleDialog::SetupSizePos(int width, int height)
{
    // Настройки окна (размер, положение)
    auto center = QGuiApplication::primaryScreen()->geometry().center();
    setGeometry(center.x() - width / 2, center.y() - height / 2, width, height);
    setFixedSize(this->size());
}
