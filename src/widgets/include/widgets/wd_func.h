#pragma once

#include <QCheckBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QValidator>
#include <gen/pch.h>
#include <gen/std_ext.h>
#include <widgets/basespinboxgroup.h>
#include <widgets/checkboxgroup.h>
#include <widgets/ecombobox.h>
#include <widgets/etableview.h>

class PasswordLineEdit;
class QMainWindow;
class QStatusBar;
class QWidget;
class UDialog;

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
    [[nodiscard]] static QString CWData(QWidget *parent, const QString &cwname);
    static bool SetCWColor(QWidget *parent, const QString &cwname, const QColor &color);
    [[nodiscard]] static QLineEdit *NewLE2(
        QWidget *parent, const QString &lename, const QString &letext = "", const QString &tooltip = "");
    [[nodiscard]] static PasswordLineEdit *NewPswLE2(
        QWidget *parent, const QString &lename, QLineEdit::EchoMode echostyle = QLineEdit::Normal);
    static bool SetLEData(QObject *parent, const QString &lename, const QString &levalue, const QString &restring = "");
    [[nodiscard]] static QString LEData(QObject *parent, const QString &lename);

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

    [[nodiscard]] static QComboBox *NewCB2(QWidget *parent, const QString &cbname, const QStringList &cbsl);
    [[nodiscard]] static QComboBox *NewCB2(QWidget *parent, const QStringList &cbsl);
    static bool SetCBData(const QWidget *parent, const QString &cbname, const QString &cbvalue);
    static bool SetCBIndex(const QObject *parent, const QString &cbname, int index);
    static bool SetCBColor(QWidget *parent, const QString &cbname, const QString &color);
    static QString CBData(const QWidget *parent, const QString &cbname);
    static int CBIndex(const QObject *parent, const QString &cbname);

    template <typename T> static T CBData(const QWidget *parent, const QString &cbname)
    {
        auto buffer = CBData(parent, cbname);
        if constexpr (std::is_floating_point_v<T>)
            buffer.replace(',', '.');
        return QVariant(buffer).value<T>();
    }

    [[nodiscard]] static QDoubleSpinBox *NewSPB2(
        QWidget *parent, const QString &spbname, const double min, const double max, const int decimals);
    [[nodiscard]] static DoubleSpinBoxGroup *NewSPBG(
        QWidget *parent, const QString &spbname, int count, const double min, const double max, const int decimals);
    [[nodiscard]] static DoubleSpinBoxGroup *NewSPBG(QWidget *parent, const QString &spbname, const QStringList &list,
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
            qWarning() << "No spinbox " << spbname;
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

    [[nodiscard]] static QLabel *NewLBL2(QWidget *parent, const QString &text, const QString &lblname = "",
        const QPixmap *pm = nullptr, const QString &lbltip = "");
    [[nodiscard]] static QLabel *NewIcon(QWidget *parent, const QString &iconpath = "");
    [[nodiscard]] static QLabel *NewLBLT2(QWidget *parent, const QString &text, const QString &lblname = "",
        const QString &lbltip = "", bool fixed = false);

    static bool SetLBLImage(QWidget *parent, const QString &lblname, QPixmap *pm);
    static bool SetLBLColor(QWidget *parent, const QString &lblname, const QString &lblcolor);
    static bool SetLBLText(QWidget *parent, const QString &lblname, const QString &lbltext = "", bool enabled = true);
    [[nodiscard]] static QString LBLText(QWidget *parent, const QString &lblname);

    [[nodiscard]] static QRadioButton *NewRB2(QWidget *parent, const QString &rbtext, const QString &rbname);
    [[nodiscard]] static QString TVField(QWidget *parent, const QString &tvname, int column, bool isid = false);
    static void TVAutoResize(QWidget *parent, const QString &tvname);

    [[nodiscard]] static QCheckBox *NewChB2(QWidget *parent, const QString &chbname, const QString &chbtext);
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
    static void AddLabelAndLineeditH(
        QWidget *parent, QHBoxLayout *hlyout, QString caption, QString lename, bool enabled = false);
    [[nodiscard]] static QWidget *NewLBLAndLBL(QWidget *parent, QString lblname, QString caption, bool enabled = false);
    [[nodiscard]] static QWidget *NewLBLAndLE(QWidget *parent, QString caption, QString lename, bool enabled = false);
    static void SetEnabled(QWidget *w, const QString &wname, bool enabled);
    static void SetVisible(QWidget *w, const QString &wname, bool visible);
    static void setMinimumSize(QWidget *w, const QString &wname, int width, int height);
    [[nodiscard]] static QString StringFloatValueWithCheck(float value, int precision = 5, bool exp = false);
    [[nodiscard]] static QVariant FloatValueWithCheck(float value);
    [[nodiscard]] static QImage *TwoImages(const QString &first, const QString &second);

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
    [[nodiscard]] static ETableView *NewTV(QWidget *parent, const QString &tvname, QAbstractItemModel *model);
    [[nodiscard]] static QTableView *NewQTV(QWidget *parent, const QString &tvname, QAbstractItemModel *model);
    [[nodiscard]] static QAbstractItemModel *TVModel(QWidget *parent, const QString &tvname);

    static void SortTV(QWidget *parent, const QString &tvname, int column, Qt::SortOrder sortorder);
    [[nodiscard]] static QVariant TVData(QWidget *parent, const QString &tvname, int column);

    [[nodiscard]] static QStatusBar *NewSB(QWidget *parent);
    [[nodiscard]] static QPixmap NewCircle(QColor color, int radius);
    [[nodiscard]] static QPixmap NewLedIndicator(QColor color, float height);
    [[nodiscard]] static QPixmap NewSVG(QString &str, QSize size);
    [[nodiscard]] static QFrame *newHLine(QWidget *parent);
    [[nodiscard]] static QFrame *newVLine(QWidget *parent);

    [[nodiscard]] static QMainWindow *getMainWindow();

    static bool floatIsWithinLimits(
        const QString &varname, double var, double base, double tolerance, bool showMessage = true);
    [[nodiscard]] static QString ChooseFileForOpen(QWidget *parent, QString mask);
    /// Input: QString mask: описание файлов, например: "Файлы журналов (*.swj)";
    /// QString ext - расширение по умолчанию Output: QString filename
    [[nodiscard]] static QString ChooseFileForSave(
        QWidget *parent, const QString &mask, const QString &ext, const QString &filename);
    [[nodiscard]] static QString ChooseFileForSave(UDialog *parent, const QString &mask, const QString &ext);
    [[nodiscard]] static QString ChooseDirectoryForOpen(QWidget *parent);

    [[nodiscard]] static QValidator *getRegExpValidator(const QString &pattern, QObject *parent = nullptr);

private:
    [[nodiscard]] static QPushButton *NewPBCommon(QWidget *parent, const QString &pbname, const QString &text,
        const QString &icon = "", const QString &pbtooltip = "");
};
