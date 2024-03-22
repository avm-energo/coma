#include "dialogfabric.h"

#include "../../../widgets/epopup.h"
#include "moduledialog.h"
#include "xml104dialog.h"
#include "xmlalarmdialog.h"
#include "xmlbsiextdialog.h"
#include "xmlconfigdialog.h"
#include "xmlhiddentabdialog.h"
#include "xmlhiddenwidgetdialog.h"
#include "xmlmeasjourdialog.h"
#include "xmlmodbusdialog.h"
#include "xmlmwidgetdialog.h"
#include "xmlprotocomdialog.h"
#include "xmlresdialog.h"
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

/// \brief Диалог создания или изменения элемента.
void XmlDialogFabric::CreateOrEditDialog(BaseEditorModel *model, int row, QWidget *parent)
{
    if (model != nullptr)
    {
        XmlDialog *dialog = nullptr;
        auto type = model->getModelType();
        switch (type)
        {
        case ModelType::Master:
            dialog = new ModuleDialog(parent);
            break;
        case ModelType::AlarmsCrit:
        case ModelType::AlarmsWarn:
        case ModelType::AlarmsInfo:
            dialog = new XmlAlarmDialog(parent);
            break;
        case ModelType::Signals:
            dialog = new XmlSignalDialog(parent);
            break;
        case ModelType::SectionTabs:
            dialog = new XmlSTabDialog(parent);
            break;
        case ModelType::WorkJours:
            dialog = new XmlWorkJourDialog(parent);
            break;
        case ModelType::MeasJours:
            dialog = new XmlMeasJourDialog(parent);
            break;
        case ModelType::Modbus:
            dialog = new XmlModbusDialog(parent);
            break;
        case ModelType::Protocom:
            dialog = new XmlProtocomDialog(parent);
            break;
        case ModelType::IEC60870:
            dialog = new Xml104Dialog(parent);
            break;
        case ModelType::Config:
            dialog = new XmlConfigDialog(parent);
            break;
        case ModelType::Sections:
            dialog = new XmlSectionDialog(parent);
            break;
        case ModelType::Section:
            dialog = new XmlSGroupDialog(parent);
            break;
        case ModelType::SGroup:
            dialog = new XmlMWidgetDialog(parent);
            break;
        case ModelType::Hidden:
            dialog = new XmlHiddenTabDialog(parent);
            break;
        case ModelType::HiddenTab:
            dialog = new XmlHiddenWidgetDialog(parent);
            break;
        case ModelType::BsiExt:
            dialog = new XmlBsiExtDialog(parent);
            break;
        case ModelType::Resources:
            if (row == createId)
                EMessageBox::warning(parent, "В данном разделе запрещено создание новых элементов");
            else
                dialog = new XmlResDialog(parent);
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
            QObject::connect(dialog, &XmlDialog::modelDataRequest, model, &BaseEditorModel::getDialogRequest);
            QObject::connect(model, &BaseEditorModel::sendDialogResponse, dialog, &XmlDialog::modelDataResponse);
            QObject::connect(dialog, &XmlDialog::createData, model, &BaseEditorModel::create);
            QObject::connect(dialog, &XmlDialog::updateData, model, &BaseEditorModel::update);
            dialog->startup(row);
            dialog->exec();
        }
    }
    else
        EMessageBox::warning(parent, "Не выбрана модель");
}

/// \brief Диалог удаления или изменения элемента.
void XmlDialogFabric::RemoveOrEditDialog(BaseEditorModel *model, QModelIndexList &selected, QWidget *parent, int type)
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
                auto modelType = model->getModelType();
                if (modelType == ModelType::Resources || modelType == ModelType::Alarms)
                    EMessageBox::warning(parent, "Выбран недопустимый элемент");
                else
                {
                    if (EMessageBox::question(parent, "Удалить выбранный элемент?"))
                        model->remove(row);
                    /*
                    auto resBtn = QMessageBox::question(parent, "Удаление", "Удалить выбранный элемент?",
                        QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
                    if (resBtn == QMessageBox::Yes)
                        model->remove(row);
                    */
                }
            }
        }
        else
            EMessageBox::warning(parent, "Выбран недопустимый элемент");
    }
    else
        EMessageBox::warning(parent, "Не выбран элемент");
}

/// \brief Создать и отобразить диалог создания нового элемента.
void XmlDialogFabric::CreateDialog(BaseEditorModel *model, QWidget *parent)
{
    CreateOrEditDialog(model, createId, parent);
}

/// \brief Создать и отобразить диалог редактирования выбранного элемента.
void XmlDialogFabric::EditDialog(BaseEditorModel *model, QModelIndexList &selected, QWidget *parent)
{
    RemoveOrEditDialog(model, selected, parent, Helper::edit);
}

/// \brief Создать и отобразить диалог удаления выбранного элемента.
void XmlDialogFabric::RemoveDialog(BaseEditorModel *model, QModelIndexList &selected, QWidget *parent)
{
    RemoveOrEditDialog(model, selected, parent, Helper::remove);
}
