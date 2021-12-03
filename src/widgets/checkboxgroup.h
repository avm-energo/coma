#pragma once
#include "../gen/s2helper.h"
#include "../gen/std_ext.h"

#include <QCheckBox>
#include <map>

class CheckBoxGroupPrivate;
class CheckBoxGroup : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CheckBoxGroup)

public:
    CheckBoxGroup(const QStringList &desc, const QList<int> &ignorePos, QWidget *parent = nullptr);
    CheckBoxGroup(const QStringList &desc, QWidget *parent = nullptr);
    CheckBoxGroup(const QStringList &desc, int count, QWidget *parent = nullptr);
    ~CheckBoxGroup() override;
    template <typename T, std::enable_if_t<std::is_unsigned_v<T>, bool> = true> void setBits(const T value);
    template <typename Container, std::enable_if_t<std_ext::is_container<Container>::value, bool> = true>
    void setBits(const Container &container)
    {
        QList<QCheckBox *> checkBoxes = findChildren<QCheckBox *>();
        for (QCheckBox *checkBox : checkBoxes)
        {
            bool status = false;
            auto number = checkBox->objectName().toUInt(&status);
            if (!status)
                continue;
            bool flag = container.at(number);
            checkBox->setChecked(flag);
        }
    }

    template <typename T, std::enable_if_t<std::is_unsigned_v<T>, bool> = true> T bits();
    template <typename Container, std::enable_if_t<std_ext::is_container<Container>::value, bool> = true>
    Container bits()
    {
        QList<QCheckBox *> checkBoxes = findChildren<QCheckBox *>();
        std::map<int, typename Container::value_type> buffer;
        for (QCheckBox *checkBox : checkBoxes)
        {
            bool status = false;
            auto number = checkBox->objectName().toUInt(&status);
            if (!status)
                continue;
            bool flag = checkBox->isChecked();
            buffer.emplace(number, flag);
        }
        Container output;
        std::transform(buffer.cbegin(), buffer.cend(), output.begin(), [](const auto &pair) { return pair.second; });
        return output;
    }

protected:
    CheckBoxGroupPrivate *const d_ptr;
    CheckBoxGroup(CheckBoxGroupPrivate &dd, QObject *parent);

private:
};

extern template void CheckBoxGroup::setBits(const quint64 value);
extern template void CheckBoxGroup::setBits(const quint32 value);
extern template void CheckBoxGroup::setBits(const quint16 value);
extern template void CheckBoxGroup::setBits(const quint8 value);

extern template quint64 CheckBoxGroup::bits();
extern template quint32 CheckBoxGroup::bits();
extern template quint16 CheckBoxGroup::bits();
extern template quint8 CheckBoxGroup::bits();
