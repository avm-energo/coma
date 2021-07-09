#pragma once
#include <QCheckBox>

class CheckBoxGroupPrivate;
class CheckBoxGroup : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CheckBoxGroup);

public:
    CheckBoxGroup(const QStringList &desc, const QList<int> &ignorePos, QWidget *parent = nullptr);
    CheckBoxGroup(const QStringList &desc, QWidget *parent = nullptr);
    ~CheckBoxGroup();
    template <typename T, std::enable_if_t<std::is_unsigned_v<T>, bool> = true> void setBits(const T value);

    template <typename T, std::enable_if_t<std::is_unsigned_v<T>, bool> = true> T bits();

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
