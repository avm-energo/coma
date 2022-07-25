#include "dialogfabric.h"

#include "../models/xmlmodel.h"
#include "xml104dialog.h"
#include "xmlalarmdialog.h"
#include "xmlconfigdialog.h"
#include "xmlmeasjourdialog.h"
#include "xmlmodbusdialog.h"
#include "xmlmwidgetdialog.h"
#include "xmlprotocomdialog.h"
#include "xmlsectiondialog.h"
#include "xmlsgroupdialog.h"
#include "xmlsignaldialog.h"
#include "xmlstabdialog.h"
#include "xmlworkjourdialog.h"

#include <QMessageBox>

void XmlDialogFabric::AnyDialog(XmlSortProxyModel *model, int row, QWidget *parent)
{
    XmlDialog *dialog = nullptr;
    auto type = qobject_cast<XmlModel *>(model->sourceModel())->getModelType();
    switch (type)
    {
    case ModelType::AlarmsItem:
        dialog = new XmlAlarmDialog(model, parent);
        break;
    case ModelType::Signals:
        dialog = new XmlSignalDialog(model, parent);
        break;
    case ModelType::SectionTabs:
        dialog = new XmlSTabDialog(model, parent);
        break;
    case ModelType::WorkJours:
        dialog = new XmlWorkJourDialog(model, parent);
        break;
    case ModelType::MeasJours:
        dialog = new XmlMeasJourDialog(model, parent);
        break;
    case ModelType::Modbus:
        dialog = new XmlModbusDialog(model, parent);
        break;
    case ModelType::Protocom:
        dialog = new XmlProtocomDialog(model, parent);
        break;
    case ModelType::IEC60870:
        dialog = new Xml104Dialog(model, parent);
        break;
    case ModelType::Config:
        dialog = new XmlConfigDialog(model, parent);
        break;
    case ModelType::Sections:
        dialog = new XmlSectionDialog(model, parent);
        break;
    case ModelType::Section:
        dialog = new XmlSGroupDialog(model, parent);
        break;
    case ModelType::SGroup:
        dialog = new XmlMWidgetDialog(model, parent);
        break;
    default:
        // TODO: What must happenes here?
        // Предположительно, ничего, как и сейчас
        break;
    }
    if (dialog != nullptr)
    {
        dialog->setupUICall(row);
    }
}

void XmlDialogFabric::CreateDialog(XmlSortProxyModel *model, QWidget *parent)
{
    AnyDialog(model, createId, parent);
}

void XmlDialogFabric::EditDialog(XmlSortProxyModel *model, QModelIndexList &selected, QWidget *parent)
{
    if (!selected.isEmpty())
    {
        auto row = selected.at(0).row();
        auto str = model->data(model->index(row, 0)).value<QString>();
        if (str != "..")
        {
            AnyDialog(model, row, parent);
        }
        else
        {
            QMessageBox::warning(parent, "Ошибка", //
                "Выбран недопустимый элемент",     //
                QMessageBox::Ok, QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::warning(parent, "Ошибка",      //
            "Не выбран элемент для редактирования", //
            QMessageBox::Ok, QMessageBox::Ok);
    }
}
