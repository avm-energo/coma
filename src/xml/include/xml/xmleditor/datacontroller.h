#pragma once

#include <QObject>
#include <QVariant>
#include <QtXml/QDomDocument>

class MasterModel;
class XmlModel;

/// \brief Контроллер данных в архитектуре MVC.
class DataController : public QObject
{
    Q_OBJECT
private:
    bool isModuleChanged;
    int changedRow;

    /// \brief Возвращает путь до файла в домашней директории пользователя.
    QString getFilePath(const QString &filename);
    /// \brief Меняет у parent узел с именем tag, на новый узел с именем tag, который содержит строку data.
    void replaceChild(QDomDocument &doc, QDomElement &parent, const QString &tag, const QVariant &data);
    /// \brief Меняет у атрибутов attrs узла node значения на vals.
    void replaceAttrs(QDomDocument &doc, QDomElement &node, const QStringList attrs, const QList<QVariant> &vals);
    /// \brief Записываем DOM документ в файл.
    /// \details Если oldName и newName не совпадают, то файл переименовывается.
    void writeToFile(const QDomDocument &doc, const QString &oldName, const QString &newName);

public:
    explicit DataController(QObject *parent = nullptr);

    /// \brief Установка флага, что файл не изменён.
    void resetOrSaved();
    /// \brief Возвращает флаг, изменён файл или нет.
    bool getModuleState() const;
    /// \brief Сохраняет номер строки изменённого конфига в мастер модели.
    int getRow() const;
    /// \brief Возвращает сохранённую строку изменённого конфига.
    void setRow(const int &row);

public slots:
    /// \brief Изменяет состояние флага, изменён ли файл, и отправляет
    /// сообщение редактору, чтобы выделить изменённый элемент.
    void configChanged();
    /// \brief Создать файл (создание конфига модуля).
    void createFile(const QStringList &creationData);
    /// \brief Переименовать файл (если было изменено имя устройства).
    void renameFile(const QString &oldName, const QString &newName);
    /// \brief Удалить файл (модуль был удалён).
    void removeFile(const QString &filename);
    /// \brief Сохранить файл (модуль открыт в правой view).
    void saveFile(MasterModel *masterModel, XmlModel *slaveModel);
    /// \brief Сохранить файл (модуль не открыт).
    void saveFile(MasterModel *masterModel);

signals:
    void highlightModified();
};
