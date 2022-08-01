#include "dialogfabric.h"

#include "../../../widgets/epopup.h"
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

namespace Helper
{
constexpr auto edit = 0;   ///< Константа для указания редактирования
constexpr auto remove = 1; ///< Константа для указания удаления
}

void XmlDialogFabric::CreateOrEditDialog(XmlSortProxyModel *model, int row, QWidget *parent)
{
    auto srcModel = qobject_cast<XmlModel *>(model->sourceModel());
    if (srcModel != nullptr)
    {
        XmlDialog *dialog = nullptr;
        auto type = srcModel->getModelType();
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
            if (row == createId)
                EMessageBox::warning(parent, "В данном разделе запрещено создание новых элементов");
            else
                EMessageBox::warning(parent, "Выбран недопустимый элемент");
            break;
        }
        if (dialog != nullptr)
        {
            dialog->setupUICall(row);
        }
    }
    else
        EMessageBox::warning(parent, "Не выбрана модель");
}

void XmlDialogFabric::RemoveOrEditDialog(XmlSortProxyModel *model, QModelIndexList &selected, QWidget *parent, int type)
{
    if (!selected.isEmpty())
    {
        auto row = selected.at(0).row();
        auto str = model->data(model->index(row, 0)).value<QString>();
        if (str != "..")
        {
            if (type == Helper::edit)
                CreateOrEditDialog(model, row, parent);
            else
            {
                auto modelType = qobject_cast<XmlModel *>(model->sourceModel())->getModelType();
                if (modelType == ModelType::Resources || modelType == ModelType::Alarms)
                    EMessageBox::warning(parent, "Выбран недопустимый элемент");
                else
                {
                    auto resBtn = QMessageBox::question(parent, "Удаление", "Удалить выбранный элемент?",
                        QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
                    if (resBtn == QMessageBox::Yes)
                        model->remove(row);
                }
            }
        }
        else
            EMessageBox::warning(parent, "Выбран недопустимый элемент");
    }
    else
        EMessageBox::warning(parent, "Не выбран элемент");
}

void XmlDialogFabric::CreateDialog(XmlSortProxyModel *model, QWidget *parent)
{
    CreateOrEditDialog(model, createId, parent);
}

void XmlDialogFabric::EditDialog(XmlSortProxyModel *model, QModelIndexList &selected, QWidget *parent)
{
    RemoveOrEditDialog(model, selected, parent, Helper::edit);
}

void XmlDialogFabric::RemoveDialog(XmlSortProxyModel *model, QModelIndexList &selected, QWidget *parent)
{
    RemoveOrEditDialog(model, selected, parent, Helper::remove);
}

void XmlDialogFabric::CreateModuleDialog(MasterModel *model)
{
    Q_UNUSED(model);
}
