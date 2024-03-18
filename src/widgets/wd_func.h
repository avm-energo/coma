#ifndef WD_FUNC
#define WD_FUNC

#include "basespinboxgroup.h"
#include "checkboxgroup.h"
#include "ecombobox.h"
#include "etableview.h"

#include <QCheckBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <gen/pch.h>
#include <gen/std_ext.h>

class PasswordLineEdit;
class QMainWindow;
class QStatusBar;
class QWidget;

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
    static bool SetCWData(QWidget *parent, const QString &cwname, const QString &cwvalue);
    static QString CWData(QWidget *parent, const QString &cwname);
    static bool SetCWColor(QWidget *parent, const QString &cwname, const QColor &color);
    static QLineEdit *NewLE2(
        QWidget *parent, const QString &lename, const QString &letext = "", const QString &tooltip = "");
    static PasswordLineEdit *NewPswLE2(
        QWidget *parent, const QString &lename, QLineEdit::EchoMode echostyle = QLineEdit::Normal);
    static bool SetLEData(QObject *parent, const QString &lename, const QString &levalue, const QString &restring = "");
    static QString LEData(QObject *parent, const QString &lename);

    template <typename T> static void LEData(QObject *parent, const QString &lename, T &value)
    {
        auto le = parent->findChild<QLineEdit *>(lename);
        if (le == nullptr)
            value = T();
        else
            value = qvariant_cast<T>(le->text());
    }

    template <typename T> static bool LENumber(QWidget *parent, const QString &lename, T &levalue)
    {
        auto le = parent->findChild<QLineEdit *>(lename);
        if (le == nullptr)
            return false;
        levalue = le->text().toDouble();
        return true;
    }

    static bool AppendTEData(QWidget *parent, const QString &tename, const QString &tetext);
    static bool SetTEData(QWidget *parent, const QString &tename, const QString &tetext);
    static bool TEData(QWidget *parent, const QString &tename, QString &tevalue);
    static bool SetLBLTColor(QWidget *parent, const QString &lblname, const QString &color);

    static QComboBox *NewCB2(QWidget *parent, const QString &cbname, const QStringList &cbsl);
    static QComboBox *NewCB2(QWidget *parent, const QStringList &cbsl);
    static bool SetCBData(const QWidget *parent, const QString &cbname, const QString &cbvalue);
    static bool SetCBIndex(const QObject *parent, const QString &cbname, int index);
    static bool SetCBColor(QWidget *parent, const QString &cbname, const QString &color);
    static QString CBData(const QWidget *parent, const QString &cbname);
    static int CBIndex(const QObject *parent, const QString &cbname);

    template <typename T> static T CBData(const QWidget *parent, const QString &cbname)
    {
        auto buffer = CBData(parent, cbname);
        return QVariant(buffer).value<T>();
    }

    static QDoubleSpinBox *NewSPB2(
        QWidget *parent, const QString &spbname, const double min, const double max, const int decimals);
    static DoubleSpinBoxGroup *NewSPBG(
        QWidget *parent, const QString &spbname, int count, const double min, const double max, const int decimals);
    static DoubleSpinBoxGroup *NewSPBG(QWidget *parent, const QString &spbname, const QStringList &list,
        const double min, const double max, const int decimals);
    static bool SetSPBData(const QObject *parent, const QString &spbname, const double &spbvalue);

    template <size_t N, typename T>
    static bool SetSPBGData(const QWidget *parent, const QString &spbname, const std::array<T, N> spbvalue)
    {
        auto spbg = static_cast<DoubleSpinBoxGroup *>(parent->findChild<QWidget *>(spbname));
        if (spbg == nullptr)
            return false;
        spbg->setValue(std::vector<float>(spbvalue.cbegin(), spbvalue.cend()));
        return true;
    }

    template <typename T>
    static bool SetSPBGData(const QWidget *parent, const QString &spbname, const std::vector<T> &spbvalue)
    {
        auto spbg = dynamic_cast<DoubleSpinBoxGroup *>(parent->findChild<QWidget *>(spbname));
        if (spbg == nullptr)
            return false;
        spbg->setValue(spbvalue);
        return true;
    }

    template <typename T>
    static bool SetSPBGData(const QWidget *parent, const QString &spbname, const QList<T> &spbvalue)
    {
        auto spbg = dynamic_cast<DoubleSpinBoxGroup *>(parent->findChild<QWidget *>(spbname));
        if (spbg == nullptr)
            return false;
        spbg->setValue(spbvalue.toVector().toStdVector());
        return true;
    }

    template <typename T> static bool SPBData(const QObject *parent, const QString &spbname, T &spbvalue)
    {
        auto spb = parent->findChild<QDoubleSpinBox *>(spbname);
        if (spb == nullptr)
        {
            spbvalue = 0;
            return false;
        }
        spbvalue = spb->value();
        return true;
    }

    template <typename T> static T SPBData(const QObject *parent, const QString &spbname)
    {
        auto spb = parent->findChild<QDoubleSpinBox *>(spbname);
        if (spb == nullptr)
        {
            qDebug() << "No spinbox " << spbname;
            return 0;
        }
        return T(spb->value());
    }

    template <size_t N, typename T>
    static bool SPBGData(const QWidget *parent, const QString &spbname, std::array<T, N> &spbvalue)
    {
        auto spbg = dynamic_cast<DoubleSpinBoxGroup *>(parent->findChild<QWidget *>(spbname));
        if (spbg == nullptr)
        {
            spbvalue = {};
            return false;
        }
        auto vector = spbg->value();
        std::copy_n(vector.cbegin(), std::min(N, vector.size()), spbvalue.begin());
        // spbvalue = spbg->value();
        return true;
    }

    static bool SetLEColor(QWidget *parent, const QString &lename, const QColor &color);
    [[deprecated("Use instead second version with global style sheet")]] static QLabel *NewLBL(QWidget *parent,
        const QString &text, const QString &lblcolor = "", const QString &lblname = "", const QPixmap *pm = Q_NULLPTR,
        const QString &lbltip = "");
    static QLabel *NewLBL2(QWidget *parent, const QString &text, const QString &lblname = "",
        const QPixmap *pm = Q_NULLPTR, const QString &lbltip = "");
    [[deprecated("Use instead second version with global style sheet")]] static QLabel *NewLBLT(QWidget *parent,
        const QString &text, const QString &lblname = "", const QString &lblstyle = "", const QString &lbltip = "",
        bool Fixed = false);
    static QLabel *NewLBLT2(QWidget *parent, const QString &text, const QString &lblname = "",
        const QString &lbltip = "", bool fixed = false);

    static bool SetLBLImage(QWidget *parent, const QString &lblname, QPixmap *pm);
    static bool SetLBLColor(QWidget *parent, const QString &lblname, const QString &lblcolor);
    static bool SetLBLText(QWidget *parent, const QString &lblname, const QString &lbltext = "", bool enabled = true);
    static QString LBLText(QWidget *parent, const QString &lblname);

    static QRadioButton *NewRB2(QWidget *parent, const QString &rbtext, const QString &rbname);
    static QString TVField(QWidget *parent, const QString &tvname, int column, bool isid = false);
    static void TVAutoResize(QWidget *parent, const QString &tvname);

    static QCheckBox *NewChB2(QWidget *parent, const QString &chbname, const QString &chbtext);
    static bool ChBData(const QWidget *parent, const QString &chbname, bool &data);
    static bool ChBData(const QWidget *parent, const QString &chbname);

    template <typename T> static bool ChBData(const QWidget *parent, const QString &chbname, T &data)
    {
        auto chb = parent->findChild<QCheckBox *>(chbname);
        if (chb == nullptr)
            return false;
        data = chb->isChecked();
        return true;
    }

    template <typename T, std::enable_if_t<std::is_unsigned_v<T> || std_ext::is_container<T>::value, bool> = true>
    static bool ChBGData(const QWidget *parent, const QString &chbname, T &data)
    {
        auto checkBoxGroup = parent->findChild<CheckBoxGroup *>(chbname);
        if (checkBoxGroup == nullptr)
            return false;
        data = checkBoxGroup->bits<std::remove_reference_t<decltype(data)>>();
        return true;
    }

    static bool SetChBData(QWidget *parent, const QString &chbname, bool data);

    template <typename T, std::enable_if_t<std::is_unsigned_v<T> || std_ext::is_container<T>::value, bool> = true>
    static bool SetChBGData(const QWidget *parent, const QString &name, const T data)
    {
        auto checkBoxGroup = parent->findChild<CheckBoxGroup *>(name);
        if (checkBoxGroup == nullptr)
        {
            qDebug() << name;
            qDebug() << parent->findChildren<QWidget *>();
            return false;
        }
        checkBoxGroup->setBits(data);
        return true;
    }

    static bool RBData(QWidget *parent, const QString &rbname, bool &data);
    static bool SetRBData(QWidget *parent, const QString &rbname, bool data);
    static bool SetIPCtrlData(const QObject *parent, const QString &name, const std::array<quint8, 4> &value);
    static std::array<quint8, 4> IPCtrlData(const QObject *parent, const QString &name);

    //    static void AddLabelAndLineedit(QLayout *lyout, QString caption, QString lename, bool enabled = false);
    static void AddLabelAndLineeditH(QWidget *parent, QHBoxLayout *hlyout, QString caption, QString lename, bool enabled = false);
    static QWidget *NewLBLAndLBL(QWidget *parent, QString lblname, QString caption, bool enabled = false);
    static QWidget *NewLBLAndLE(QWidget *parent, QString caption, QString lename, bool enabled = false);
    static void SetEnabled(QWidget *w, const QString &wname, bool enabled);
    static void SetVisible(QWidget *w, const QString &wname, bool visible);
    static void setMinimumSize(QWidget *w, const QString &wname, int width, int height);
    static QString StringFloatValueWithCheck(float value, int precision = 5, bool exp = false);
    static QVariant FloatValueWithCheck(float value);
    static QImage *TwoImages(const QString &first, const QString &second);

    template <typename Functor>
    inline static QPushButton *NewHexagonPB(QWidget *parent, const QString &pbname, Functor &&functor,
        const QString &icon = "", const QString &pbtooltip = "")
    {
        auto pb = new QPushButton(parent);
        pb->setObjectName(pbname);
        pb->setIcon(QIcon(icon));
        pb->setAttribute(Qt::WA_Hover);
        pb->setAttribute(Qt::WA_X11NetWmWindowTypeToolBar);
        pb->setToolTip(pbtooltip);
        pb->setMinimumSize(50, 50);
        pb->setIconSize(QSize(50, 50));
        QObject::connect(pb, &QPushButton::clicked, functor);
        return pb;
    }

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

    template <typename T> //
    inline static T *GetChildFromParents(const QList<QWidget *> &parents, const QString &name)
    {
        for (auto &parent : parents)
        {
            auto child = parent->findChild<T *>(name);
            if (child != nullptr)
                return child;
        }
        return nullptr;
    }

    static void SetTVModel(QWidget *parent, const QString &tvname, QAbstractItemModel *model, bool sortenable = false);
    static void SetQTVModel(QWidget *parent, const QString &tvname, QAbstractItemModel *model, bool sortenable = false);
    static ETableView *NewTV(QWidget *parent, const QString &tvname, QAbstractItemModel *model);
    static QTableView *NewQTV(QWidget *parent, const QString &tvname, QAbstractItemModel *model);
    static QAbstractItemModel *TVModel(QWidget *parent, const QString &tvname);

    static void SortTV(QWidget *parent, const QString &tvname, int column, Qt::SortOrder sortorder);
    static QVariant TVData(QWidget *parent, const QString &tvname, int column);

    static QStatusBar *NewSB(QWidget *parent);
    static QPixmap NewCircle(QColor color, int radius);
    static QPixmap NewLedIndicator(QColor color, float height);
    static QPixmap NewSVG(QString &str, QSize size);
    static QFrame *newHLine(QWidget *parent);
    static QFrame *newVLine(QWidget *parent);

    static QMainWindow *getMainWindow();

    static bool floatIsWithinLimits(
        const QString &varname, double var, double base, double tolerance, bool showMessage = true);
    static QString ChooseFileForOpen(QWidget *parent, QString mask);
    /// Input: QString mask: описание файлов, например: "Файлы журналов (*.swj)";
    /// QString ext - расширение по умолчанию Output: QString filename
    static QString ChooseFileForSave(
        QWidget *parent, const QString &mask, const QString &ext, const QString &filenamestr = "");
    static QString ChooseDirectoryForOpen(QWidget *parent);

private:
    static QPushButton *NewPBCommon(QWidget *parent, const QString &pbname, const QString &text,
        const QString &icon = "", const QString &pbtooltip = "");
};

#endif // WD_FUNC
