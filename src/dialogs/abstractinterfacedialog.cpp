#include "abstractinterfacedialog.h"

#include <QLayout>
#include <QPushButton>
#include <QSettings>
#include <memory>
AbstractInterfaceDialog::AbstractInterfaceDialog(QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

void AbstractInterfaceDialog::setupUI()
{
    QPushButton *pb = new QPushButton("Отмена");
    connect(pb, &QAbstractButton::clicked, this, &QDialog::close);
    QVBoxLayout *lyout = static_cast<QVBoxLayout *>(layout());
    // lyout->addStretch(20);
    lyout->addWidget(pb);
    // lyout->addStretch(20);
}

void AbstractInterfaceDialog::rotateSettings(const QString &type, const QString &name)
{
    auto settings = std::unique_ptr<QSettings>(new QSettings);
    QStringList sl;
    QString namename, oldnamename;
    // 1. get all type+'i' from registry (count)
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        namename = type + QString::number(i);
        QString value = settings->value(namename, "").toString();
        if (!value.isEmpty())
            sl << value;
    }
    // 2. if count < 5 then return count
    if (sl.size() < MAXREGISTRYINTERFACECOUNT)
        sl.append("");
    else
    {
        // 3. else delete group "type + sl.size()-1"
        namename = type + QString::number(sl.size() - 1);
        settings->remove(settings->value(namename, "").toString());
    }
    // and rotate it (1->0, 2->1 etc)
    for (int i = (sl.size() - 1); i > 0; --i)
    {
        oldnamename = type + QString::number(i);
        namename = type + QString::number(i - 1);
        settings->setValue(oldnamename, settings->value(namename, ""));
    }
    namename = type + "0";
    settings->setValue(namename, name);
}

bool AbstractInterfaceDialog::isKeyExist(const QString &type, const QString &chstr)
{
    auto settings = std::unique_ptr<QSettings>(new QSettings);
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        QString key = type + QString::number(i);
        if (settings->value(key, "").toString() == chstr)
            return true;
    }
    return false;
}

void AbstractInterfaceDialog::removeDevice(const QString &name)
{
    auto settings = std::unique_ptr<QSettings>(new QSettings);
    settings->remove(name);
    const auto keys = settings->allKeys();
    for (const auto &key : keys)
    {
        auto storedValue = settings->value(key).toString();
        if (storedValue == name)
            settings->remove(key);
    }
}
