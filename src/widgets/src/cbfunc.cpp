#include <widgets/cbfunc.h>

#include <QStringListModel>

CBFunc::CBFunc() { }

QComboBox *CBFunc::NewCB2(QWidget *parent, const QString &cbname, const QStringList &cbsl)
{
    auto cb = NewCB2(parent, cbsl);
    cb->setObjectName(cbname);
    return cb;
}

QComboBox *CBFunc::NewCB2(QWidget *parent, const QStringList &cbsl)
{
    auto cb = new QComboBox(parent);
    auto cblm = new QStringListModel(cbsl, cb);
    cb->setModel(cblm);
    return cb;
}

QString CBFunc::CBData(const QWidget *parent, const QString &cbname)
{
    auto cb = parent->findChild<QComboBox *>(cbname);
    if (cb == nullptr)
        return QString();
    return cb->currentText();
}

int CBFunc::CBIndex(const QObject *parent, const QString &cbname)
{
    auto cb = parent->findChild<QComboBox *>(cbname);
    if (cb == nullptr)
        return -1;
    return cb->currentIndex();
}

bool CBFunc::SetCBData(const QWidget *parent, const QString &cbname, const QString &cbvalue)
{
    auto cb = parent->findChild<QComboBox *>(cbname);
    if (cb == nullptr)
        return false;
    cb->setCurrentText(cbvalue);
    return true;
}

bool CBFunc::SetCBIndex(const QObject *parent, const QString &cbname, int index)
{
    auto cb = parent->findChild<QComboBox *>(cbname);
    if (cb == nullptr)
        return false;
    if (index < cb->count())
    {
        cb->setCurrentIndex(index);
        return true;
    }

    return false;
}

bool CBFunc::SetCBColor(QWidget *parent, const QString &cbname, const QString &color)
{
    auto cb = parent->findChild<QComboBox *>(cbname);
    if (cb == nullptr)
        return false;
    // http://forum.sources.ru/index.php?showtopic=313950
    auto pal = cb->palette();
    pal.setColor(QPalette::Text, QColor(color));
    cb->setPalette(pal);
    return true;
}
