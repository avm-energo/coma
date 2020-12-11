#ifndef WD_FUNC
#define WD_FUNC

#include "ecombobox.h"
#include "edoublespinbox.h"
#include "etableview.h"
#include "passwordlineedit.h"

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QStatusBar>
#include <QWidget>

#ifndef __GNUC__
#define MAXFLOAT 3.40282347E+38F
#endif
class WDFunc
{
public:
    enum CBSignals
    {
        CT_TEXTCHANGED,
        CT_INDEXCHANGED,
        CT_CLICKED,
        CT_DCLICKED,
        CT_CONTEXT
    };

    WDFunc();
    static bool SetCWData(QWidget *w, const QString &cwname, const QString &cwvalue);
    static QString CWData(QWidget *w, const QString &cwname);
    static bool SetCWColor(QWidget *w, const QString &cwname, const QColor &color);
    static QLineEdit *NewLE(QWidget *w, const QString &lename, const QString &letext = "", const QString &lestyle = "");
    static PasswordLineEdit *NewPswLE(QWidget *w, const QString &lename,
        QLineEdit::EchoMode echostyle = QLineEdit::Normal, const QString &lestyle = "");
    static bool SetLEData(QWidget *w, const QString &lename, const QString &levalue, const QString &restring = "");
    static QString LEData(QObject *w, const QString &lename);
    template <typename T> static bool LENumber(QWidget *w, const QString &lename, T &levalue)
    {
        QLineEdit *le = w->findChild<QLineEdit *>(lename);
        if (le == nullptr)
            return false;
        levalue = le->text().toDouble();
        return true;
    }
    static bool AppendTEData(QWidget *w, const QString &tename, const QString &tetext);
    static bool SetTEData(QWidget *w, const QString &tename, const QString &tetext);
    static bool TEData(QWidget *w, const QString &tename, QString &tevalue);
    static bool SetLBLTColor(QWidget *w, const QString &lblname, const QString &color);
    static EComboBox *NewCB(
        QWidget *parent, const QString &cbname, const QStringList &cbsl, const QString &cbcolor = "");
    static bool SetCBData(QWidget *w, const QString &cbname, const QString &cbvalue);
    static bool SetCBIndex(QObject *w, const QString &cbname, int index);
    static bool SetCBColor(QWidget *w, const QString &cbname, const QString &color);
    static QString CBData(QWidget *w, const QString &cbname);
    static int CBIndex(QObject *w, const QString &cbname)
    {
        EComboBox *cb = w->findChild<EComboBox *>(cbname);
        if (cb == nullptr)
            return -1;
        return cb->currentIndex();
    }
    //    static QMetaObject::Connection CBConnect(
    //        QWidget *w, const QString &cbname, int cbconnecttype, const QObject *receiver, const char *method);

    static EDoubleSpinBox *NewSPB(
        QWidget *parent, const QString &spbname, double min, double max, int decimals, const QString &spbcolor = "");

    static bool SetSPBData(QObject *w, const QString &spbname, const double &spbvalue);
    template <typename T> static bool SPBData(QObject *w, const QString &spbname, T &spbvalue)
    {
        QDoubleSpinBox *spb = w->findChild<QDoubleSpinBox *>(spbname);
        if (spb == nullptr)
        {
            spbvalue = 0;
            return false;
        }
        spbvalue = spb->value();
        return true;
    }
    static bool SetLEColor(QWidget *w, const QString &lename, const QColor &color);
    static QLabel *NewLBL(QWidget *w, const QString &text, const QString &lblcolor = "", const QString &lblname = "",
        const QPixmap *pm = Q_NULLPTR, const QString &lbltip = "");
    static QLabel *NewLBLT(QWidget *w, const QString &text, const QString &lblname = "", const QString &lblstyle = "",
        const QString &lbltip = "", bool Fixed = false);
    // static QLabel *NewLBLTT(QWidget *w, const QString &text, const QString
    // &lblname="", const QString &lblstyle="", const QString &lbltip="",bool
    // Fixed=false);
    static bool SetLBLImage(QWidget *w, const QString &lblname, QPixmap *pm);
    static bool SetLBLColor(QWidget *w, const QString &lblname, const QString &lblcolor);
    static bool SetLBLText(QWidget *w, const QString &lblname, const QString &lbltext = "", bool enabled = true);
    static QString LBLText(QWidget *w, const QString &lblname);
    static QRadioButton *NewRB(
        QWidget *parent, const QString &rbtext, const QString &rbname, const QString &rbcolor = "");
    static QString TVField(QWidget *w, const QString &tvname, int column, bool isid = false);
    static void TVAutoResize(QWidget *w, const QString &tvname);
    static QCheckBox *NewChB(
        QWidget *parent, const QString &chbname, const QString &chbtext, const QString &chbcolor = "");
    static bool ChBData(QWidget *w, const QString &chbname, bool &data);
    static bool SetChBData(QWidget *w, const QString &chbname, bool data);
    static bool RBData(QWidget *w, const QString &rbname, bool &data);
    static bool SetRBData(QWidget *w, const QString &rbname, bool data);
    static void AddLabelAndLineedit(QLayout *lyout, QString caption, QString lename, bool enabled = false);
    static void AddLabelAndLineeditH(QLayout *lyout, QString caption, QString lename, bool enabled = false);
    static QWidget *NewLBLAndLE(QWidget *parent, QString caption, QString lename, bool enabled = false);
    static void SetEnabled(QWidget *w, const QString &wname, bool enabled);
    static void SetVisible(QWidget *w, const QString &wname, bool visible);
    static void setMinimumSize(QWidget *w, const QString &wname, int width, int height);
    static QString StringValueWithCheck(float value, int precision = 5, bool exp = false);
    static QVariant FloatValueWithCheck(float value);
    static QImage *TwoImages(const QString &first, const QString &second);
    //    static QPushButton *NewPB(QWidget *parent, const QString &pbname, const QString &text, const QObject
    //    *receiver,
    //        const char *method, const QString &icon = "", const QString &pbtooltip = "");

    template <typename Func>
    inline static QPushButton *NewPB(QWidget *parent, const QString &pbname, const QString &text, const Func *receiver,
        void (Func::*method)(), const QString &icon = "", const QString &pbtooltip = "")
    {
        QPushButton *pb = NewPBCommon(parent, pbname, text, icon, pbtooltip);
        QObject::connect(pb, &QPushButton::clicked, receiver, method);
        return pb;
    }
    template <typename Functor>
    inline static QPushButton *NewPB(QWidget *parent, const QString &pbname, const QString &text, Functor &&functor,
        const QString &icon = "", const QString &pbtooltip = "")
    {
        QPushButton *pb = NewPBCommon(parent, pbname, text, icon, pbtooltip);
        QObject::connect(pb, &QPushButton::clicked, functor);
        return pb;
    }
    template <typename Functor>
    inline static QPushButton *NewPB(QWidget *parent, const QString &pbname, const QString &text, QObject *context,
        Functor &&functor, const QString &icon = "", const QString &pbtooltip = "")
    {
        QPushButton *pb = NewPBCommon(parent, pbname, text, icon, pbtooltip);
        QObject::connect(pb, &QPushButton::clicked, context, functor);
        return pb;
    }
    template <typename Functor>
    inline static void PBConnect(QWidget *parent, const QString &pbname, QObject *context, Functor &&functor)
    {
        QPushButton *pb = parent->findChild<QPushButton *>(pbname);
        if (pb != nullptr)
            QObject::connect(pb, &QPushButton::clicked, context, functor);
    }
    template <typename Object>
    inline static void PBConnect(
        QWidget *parent, const QString &pbname, const Object *receiver, void (Object::*method)())
    {
        QPushButton *pb = parent->findChild<QPushButton *>(pbname);
        if (pb != nullptr)
            QObject::connect(pb, &QPushButton::clicked, receiver, method);
    }

    //    static QMetaObject::Connection PBConnect(
    //        QWidget *w, const QString &pbname, const QObject *receiver, const char *method);
    static void SetTVModel(QWidget *w, const QString &tvname, QAbstractItemModel *model, bool sortenable = false);
    static void SetQTVModel(QWidget *w, const QString &tvname, QAbstractItemModel *model, bool sortenable = false);
    static ETableView *NewTV(QWidget *w, const QString &tvname, QAbstractItemModel *model);
    static QTableView *NewQTV(QWidget *w, const QString &tvname, QAbstractItemModel *model);
    static QAbstractItemModel *TVModel(QWidget *w, const QString &tvname);
    //    static void TVConnect(
    //        QWidget *w, const QString &tvname, int signaltype, const QObject *receiver, const char *method);
    static bool LE_read_data(QObject *w, const QString &lename, QString &levalue);
    static bool LE_write_data(QObject *w, const QString &levalue, const QString &lename);
    static void SortTV(QWidget *w, const QString &tvname, int column, Qt::SortOrder sortorder);
    static QVariant TVData(QWidget *w, const QString &tvname, int column);

    static QStatusBar *NewSB(QWidget *w);
    static QPixmap NewCircle(QColor color, int radius);
    static QPixmap NewLedIndicator(QColor color, float height);
    static QPixmap NewSVG(QString &str, QSize size);

private:
    static QPushButton *NewPBCommon(QWidget *parent, const QString &pbname, const QString &text,
        const QString &icon = "", const QString &pbtooltip = "")
    {
        QPushButton *pb = new QPushButton(parent);
        pb->setStyleSheet("QPushButton {background-color: rgba(0,0,0,0); border: 1px solid gray; "
                          "border-radius: 5px; border-style: outset; padding: 2px 5px;}"
                          "QPushButton:pressed {border-style: inset;}"
                          "QPushButton:disabled {border: none;}");
        pb->setObjectName(pbname);
        if (!icon.isEmpty())
            pb->setIcon(QIcon(icon));
        pb->setText(text);
        pb->setToolTip(pbtooltip);
        return pb;
    }
};

#endif // WD_FUNC
