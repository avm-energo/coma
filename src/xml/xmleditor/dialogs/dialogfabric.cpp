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

namespace Helper
{
constexpr auto edit = 0;   ///< Константа для указания редактирования
constexpr auto remove = 1; ///< Константа для указания удаления

/// \brief Вспомогательная функция для вывода MessageBox с Warning сообщением
inline void WarningMessage(QString &&text, QWidget *parent = nullptr)
{
    QMessageBox::warning(parent, "Ошибка", text, //
        QMessageBox::Ok, QMessageBox::Ok);
}
}

void XmlDialogFabric::Dialog(XmlSortProxyModel *model, int row, QWidget *parent)
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
        if (row == createId)
            Helper::WarningMessage("В данном разделе запрещено создание новых элементов", parent);
        else
            Helper::WarningMessage("Выбран недопустимый элемент", parent);
        break;
    }
    if (dialog != nullptr)
    {
        dialog->setupUICall(row);
    }
}

void XmlDialogFabric::Dialog(XmlSortProxyModel *model, QModelIndexList &selected, QWidget *parent, int type)
{
    if (!selected.isEmpty())
    {
        auto row = selected.at(0).row();
        auto str = model->data(model->index(row, 0)).value<QString>();
        if (str != "..")
        {
            if (type == Helper::edit)
                Dialog(model, row, parent);
            else
            {
                auto modelType = qobject_cast<XmlModel *>(model->sourceModel())->getModelType();
                if (modelType == ModelType::Resources || modelType == ModelType::Alarms)
                    Helper::WarningMessage("Выбран недопустимый элемент", parent);
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
            Helper::WarningMessage("Выбран недопустимый элемент", parent);
    }
    else
        Helper::WarningMessage("Не выбран элемент", parent);
}

void XmlDialogFabric::CreateDialog(XmlSortProxyModel *model, QWidget *parent)
{
    Dialog(model, createId, parent);
}

void XmlDialogFabric::EditDialog(XmlSortProxyModel *model, QModelIndexList &selected, QWidget *parent)
{
    Dialog(model, selected, parent, Helper::edit);
}

void XmlDialogFabric::RemoveDialog(XmlSortProxyModel *model, QModelIndexList &selected, QWidget *parent)
{
    Dialog(model, selected, parent, Helper::remove);
}
