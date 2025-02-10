#include <widgets/pbfunc.h>

PBFunc::PBFunc() { }

QPushButton *PBFunc::NewPBCommon(
    QWidget *parent, const QString &pbname, const QString &text, const QString &icon, const QString &pbtooltip)
{
    auto pb = new QPushButton(text, parent);
    pb->setObjectName(pbname);
    if (!icon.isEmpty())
        pb->setIcon(QIcon(icon));
    pb->setToolTip(pbtooltip);
    return pb;
}
