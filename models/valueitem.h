#ifndef VALUEITEM_H
#define VALUEITEM_H

#include <QObject>

class SpbDataStyleAdditionalDataClass : public QObject
{
public:
    SpbDataStyleAdditionalDataClass() = default;
    ~SpbDataStyleAdditionalDataClass() = default;
    SpbDataStyleAdditionalDataClass(const SpbDataStyleAdditionalDataClass &cpy);
    SpbDataStyleAdditionalDataClass &operator=(const SpbDataStyleAdditionalDataClass &cpy);

    double min;
    double max;
    int decimals;
};

class ValueItem : public QObject
{
    Q_OBJECT
public:
    enum DataStyles
    {
        Label,
        LineEdit,
        SpinBox,
        ComboBox
    };

    enum DataFormat
    {
        SIMPLE_CELL = 0, // general data output
        OUTVALUEINT = 1, // int
        OUTVALUEFLOAT0 = 2, // 0 digits after zero
        OUTVALUEFLOAT1 = 3,
        OUTVALUEFLOAT2 = 4,
        OUTVALUEFLOAT3 = 5,
        OUTVALUEFLOAT4 = 6,
        OUTVALUEFLOAT5 = 7, // 5 digits after zero
        OUTVALUESTRING = 8, // abracadabra
        OUTVALUEDOUBLE = 9, // 16e-018
        OUTVALUEHEX = 10 // 0x5af3
    };

    //    explicit ValueItem(QObject *parent = nullptr);
    ValueItem() = default;
    ~ValueItem() = default;
    ValueItem(const ValueItem &);
    ValueItem &operator=(const ValueItem &item);
    template <typename T> void *newVar(const void *oldvar)
    {
        T *sl = static_cast<T *>(oldvar);
        T *newsl = new T;
        *newsl = *sl;
        return newsl;
    }

    template <typename T> void setVar(void *newvar, const void *oldvar)
    {
        T *sl = static_cast<T *>(oldvar);
        T *newsl = static_cast<T *>(newvar);
        *newsl = *sl;
    }

    void *dataPtr();
    void setDataPtr(void *ptr);
    QString name();
    void setName(const QString &str);
    QString tooltip();
    void setTooltip(const QString &str);
    DataStyles style();
    void setStyle(DataStyles stl);
    DataFormat format();
    void setFormat(DataFormat fmt);
    void *addData();
    void setAddData(void *ptr);

signals:

private:
    void *m_dataPtr; // pointer to the variable with data
    QString m_name; // label name
    QString m_tooltip; // tooltip on the screen
    DataStyles m_style; // style
    DataFormat m_format;
    void *m_addData; // for ComboBoxes - QStringList *, for SpinBoxes - SpbDataStyleAdditionalDataClass *
};

Q_DECLARE_METATYPE(ValueItem);

#endif // VALUEITEM_H
