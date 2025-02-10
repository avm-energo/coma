#pragma once

#include <QPushButton>

class PBFunc
{
public:
    PBFunc();

    template <typename Func>
    inline static QPushButton *NewPB(QWidget *parent, const QString &pbname, const QString &text, const Func *receiver,
        void (Func::*method)(), const QString &icon = "", const QString &pbtooltip = "")
    {
        auto pb = NewPBCommon(parent, pbname, text, icon, pbtooltip);
        QObject::connect(pb, &QPushButton::clicked, receiver, method);
        return pb;
    }

    template <typename Functor>
    inline static QPushButton *NewPB(QWidget *parent, const QString &pbname, const QString &text, Functor &&functor,
        const QString &icon = "", const QString &pbtooltip = "")
    {
        auto pb = NewPBCommon(parent, pbname, text, icon, pbtooltip);
        QObject::connect(pb, &QPushButton::clicked, functor);
        return pb;
    }

    template <typename Functor>
    inline static QPushButton *NewPB(QWidget *parent, const QString &pbname, const QString &text, QObject *context,
        Functor &&functor, const QString &icon = "", const QString &pbtooltip = "")
    {
        auto pb = NewPBCommon(parent, pbname, text, icon, pbtooltip);
        QObject::connect(pb, &QPushButton::clicked, context, functor);
        return pb;
    }

    template <typename Functor>
    inline static void PBConnect(QWidget *parent, const QString &pbname, QObject *context, Functor &&functor)
    {
        auto pb = parent->findChild<QPushButton *>(pbname);
        if (pb != nullptr)
            QObject::connect(pb, &QPushButton::clicked, context, functor);
    }

    template <typename Object>
    inline static void PBConnect(
        QWidget *parent, const QString &pbname, const Object *receiver, void (Object::*method)())
    {
        auto pb = parent->findChild<QPushButton *>(pbname);
        if (pb != nullptr)
            QObject::connect(pb, &QPushButton::clicked, receiver, method);
    }

private:
    [[nodiscard]] static QPushButton *NewPBCommon(QWidget *parent, const QString &pbname, const QString &text,
        const QString &icon = "", const QString &pbtooltip = "");
};
