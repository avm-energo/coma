#pragma once
#include <widgets/hexagonbutton.h>

class HexPBFunc
{
public:
    HexPBFunc();

    template <typename Functor>
    inline static HexagonButton *NewHexagonPB(QWidget *parent, const QString &pbname, Functor &&functor,
        const QString &icon = "", const QString &pbtooltip = "")
    {
        auto pb = new HexagonButton(parent);
        pb->setObjectName(pbname);
        pb->setIcon(QIcon(icon));
        pb->setData(icon);
        pb->setToolTip(pbtooltip);
        pb->setMinimumSize(50, 50);
        pb->setIconSize(QSize(50, 50));
        QObject::connect(pb, &QPushButton::clicked, functor);
        return pb;
    }

    static void setHexagonPBProcessed(QWidget *parent, const QString &name);
    static void setHexagonPBRestricted(QWidget *parent, const QString &name);
    static void setHexagonPBNormal(QWidget *parent, const QString &name);
    static void setHexagonPBIcon(QWidget *parent, const QString &name, QStringList &attrs, QStringList &values);
};
