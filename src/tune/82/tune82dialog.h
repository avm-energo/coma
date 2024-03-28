#ifndef TUNE82DIALOG_H
#define TUNE82DIALOG_H

#include "../../models/report.h"
#include "../generaltunedialog.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>
#include <device/device_list.h>

class Tune82Dialog : public GeneralTuneDialog
{
    Q_OBJECT
public:
    explicit Tune82Dialog(Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    void prepareReport() override;

private slots:
};

#endif // TUNE82DIALOG_H
