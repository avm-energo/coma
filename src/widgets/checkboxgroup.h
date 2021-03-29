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
    template <typename T> void setBits(const T value);

    template <typename T> T bits();

protected:
    CheckBoxGroupPrivate *const d_ptr;
    CheckBoxGroup(CheckBoxGroupPrivate &dd, QObject *parent);

private:
};
template <> void CheckBoxGroup::setBits(const quint64 value);
template <> void CheckBoxGroup::setBits(const quint32 value);
template <> quint32 CheckBoxGroup::bits();
template <> quint64 CheckBoxGroup::bits();
