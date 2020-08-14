#ifndef WD_FUNC
#define WD_FUNC

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>

#include "../models/etablemodel.h"
#include "ecombobox.h"
#include "etableview.h"
#include "passwordlineedit.h"

#define MAXFLOAT 3.40282347E+38F

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
    static QString LEData(QWidget *w, const QString &lename);
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
    static EComboBox *NewCB(QWidget *parent, const QString &cbname, QStringList &cbsl, const QString &cbcolor = "");
    static bool SetCBData(QWidget *w, const QString &cbname, const QString &cbvalue);
    static bool SetCBIndex(QWidget *w, const QString &cbname, int index);
    static bool SetCBColor(QWidget *w, const QString &cbname, const QString &color);
    static QString CBData(QWidget *w, const QString &cbname);
    static int CBIndex(QWidget *w, const QString &cbname)
    {
        EComboBox *cb = w->findChild<EComboBox *>(cbname);
        if (cb == nullptr)
            return -1;
        return cb->currentIndex();
    }
    static QMetaObject::Connection CBConnect(
        QWidget *w, const QString &cbname, int cbconnecttype, const QObject *receiver, const char *method);

    static QDoubleSpinBox *NewSPB(
        QWidget *parent, const QString &spbname, double min, double max, int decimals, const QString &spbcolor = "");
    static bool SetSPBData(QWidget *w, const QString &spbname, const double &spbvalue);
    template <typename T> static bool SPBData(QWidget *w, const QString &spbname, T &spbvalue)
    {
        QDoubleSpinBox *spb = w->findChild<QDoubleSpinBox *>(spbname);
        if (spb == nullptr)
            return false;
        spbvalue = spb->value();
        return true;
    }
    static bool SetLEColor(QWidget *w, const QString &lename, const QColor &color);
    static QLabel *NewLBL(QWidget *w, const QString &text, const QString &lblcolor = "", const QString &lblname = "",
        const QPixmap *pm = Q_NULLPTR);
    static QLabel *NewLBLT(QWidget *w, const QString &text, const QString &lblname = "", const QString &lblstyle = "",
        const QString &lbltip = "");
    static bool SetLBLImage(QWidget *w, const QString &lblname, QPixmap *pm);
    static bool SetLBLColor(QWidget *w, const QString &lblname, const QString &lblcolor);
    static bool SetLBLText(QWidget *w, const QString &lblname, const QString &lbltext = "", bool enabled = true);
    static bool LBLText(QWidget *w, const QString &lblname, QString &text);
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
    static QString StringValueWithCheck(float value, int precision = 5);
    static QVariant FloatValueWithCheck(float value);
    static QImage *TwoImages(const QString &first, const QString &second);
    static QPushButton *NewPB(QWidget *parent, const QString &pbname, const QString &text, const QObject *receiver,
        const char *method, const QString &icon = "", const QString &pbtooltip = "");
    static QMetaObject::Connection PBConnect(
        QWidget *w, const QString &pbname, const QObject *receiver, const char *method);
    static void SetTVModel(QWidget *w, const QString &tvname, QAbstractItemModel *model, bool sortenable = false);
    static ETableView *NewTV(QWidget *w, const QString &tvname, QAbstractItemModel *model);
    static QAbstractItemModel *TVModel(QWidget *w, const QString &tvname);
    static void TVConnect(
        QWidget *w, const QString &tvname, int signaltype, const QObject *receiver, const char *method);
    static bool LE_read_data(QWidget *w, const QString &lename, QString &levalue);
    static bool LE_write_data(QWidget *w, const QString &levalue, const QString &lename);
    static void SortTV(QWidget *w, const QString &tvname, int column, Qt::SortOrder sortorder);
    static QVariant TVData(QWidget *w, const QString &tvname, int column);
};

#endif // WD_FUNC
