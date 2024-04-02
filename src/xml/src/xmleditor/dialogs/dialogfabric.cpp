#include "xml/xmleditor/dialogs/dialogfabric.h"

#include <QMessageBox>
#include <widgets/epopup.h>
#include <xml/xmleditor/dialogs/moduledialog.h>
#include <xml/xmleditor/dialogs/xml104dialog.h>
#include <xml/xmleditor/dialogs/xmlalarmdialog.h>
#include <xml/xmleditor/dialogs/xmlalarmstatealldialog.h>
#include <xml/xmleditor/dialogs/xmlbsiextdialog.h>
#include <xml/xmleditor/dialogs/xmlconfigdialog.h>
#include <xml/xmleditor/dialogs/xmlhiddentabdialog.h>
#include <xml/xmleditor/dialogs/xmlhiddenwidgetdialog.h>
#include <xml/xmleditor/dialogs/xmlmeasjourdialog.h>
#include <xml/xmleditor/dialogs/xmlmodbusdialog.h>
#include <xml/xmleditor/dialogs/xmlmwidgetdialog.h>
#include <xml/xmleditor/dialogs/xmlprotocomdialog.h>
#include <xml/xmleditor/dialogs/xmlresdialog.h>
#include <xml/xmleditor/dialogs/xmlsectiondialog.h>
#include <xml/xmleditor/dialogs/xmlsgroupdialog.h>
#include <xml/xmleditor/dialogs/xmlsignaldialog.h>
#include <xml/xmleditor/dialogs/xmlstabdialog.h>
#include <xml/xmleditor/dialogs/xmlworkjourdialog.h>

namespace Helper
{
constexpr auto edit = 0;   ///< Константа для указания редактирования.
constexpr auto remove = 1; ///< Константа для указания удаления.
}

void XmlDialogFabric::createOrEditDialog(BaseEditorModel *model, int row, QWidget *parent)
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
        case ModelType::AlarmStateAll:
            dialog = new XmlAlarmStateAllDialog(parent);
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
            dialog = new XmlSectionTabDialog(parent);
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
            QObject::connect(dialog, &XmlDialog::createData, model, &BaseEditorModel::create);
            QObject::connect(dialog, &XmlDialog::updateData, model, &BaseEditorModel::update);
            dialog->startup(row, model);
            dialog->exec();
        }
    }
    else
        EMessageBox::warning(parent, "Не выбрана модель");
}

void XmlDialogFabric::removeOrEditDialog(BaseEditorModel *model, QModelIndexList &selected, QWidget *parent, int type)
{
    if (!selected.isEmpty())
    {
        auto row = selected.at(0).row();
        auto str = model->data(model->index(row, 0)).value<QString>();
        if (str != "..")
        {
            if (type == Helper::edit)
                createOrEditDialog(model, row, parent);
            else
            {
                auto modelType = model->getModelType();
                if (modelType == ModelType::Resources || modelType == ModelType::Alarms)
                    EMessageBox::warning(parent, "Выбран недопустимый элемент");
                else
                {
                    if (EMessageBox::question(parent, "Удалить выбранный элемент?"))
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

void XmlDialogFabric::createDialog(BaseEditorModel *model, QWidget *parent)
{
    createOrEditDialog(model, createId, parent);
}

void XmlDialogFabric::editDialog(BaseEditorModel *model, QModelIndexList &selected, QWidget *parent)
{
    removeOrEditDialog(model, selected, parent, Helper::edit);
}

void XmlDialogFabric::removeDialog(BaseEditorModel *model, QModelIndexList &selected, QWidget *parent)
{
    removeOrEditDialog(model, selected, parent, Helper::remove);
}
