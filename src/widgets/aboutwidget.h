#ifndef ABOUTWIDGET_H
#define ABOUTWIDGET_H

#include <QPaintEvent>
#include <QWidget>

namespace AboutText
{
constexpr char name_ru[] = "АВМ-Сервис";
constexpr char department[] = "ООО \"АВМ-Энерго\", 2016-2020";
constexpr char modules[] = "Modules used:";
constexpr char zlib[] = "Data Compression Library (zlib), version 1.2.11 (http://zlib.net/)";
constexpr char hidapi[] = "HIDAPI library, version 0.9.0 (https://github.com/libusb/hidapi)";
constexpr char qxlsx[] = "QXlsx library, version 1.3.56 (https://qtexcel.github.io/QXlsx/)";
constexpr char limereport[] = "LimeReport, version 1.5.1 (http://limereport.ru/)";
constexpr char icons[] = "Icons are based on the iconset by 0melapics / Freepik";
constexpr char fonts[] = "Font used: FlorenceSans by ShyFoundry (http://www.shyfoundry.com)";
}

class AboutWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AboutWidget(QWidget *parent = nullptr);

signals:

protected:
    //    void paintEvent(QPaintEvent *e);
};

#endif // ABOUTWIDGET_H
