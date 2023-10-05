#pragma once

#include <QSplashScreen>

// Сделать translatable
namespace Prog
{
constexpr char name_ru[] = "АВМ-Сервис";
constexpr char department[] = "ООО \"АВМ-Энерго\", 2016-2021";
constexpr char modules[] = "Modules used:";
constexpr char zlib[] = "Data Compression Library (zlib), version 1.2.11 (http://zlib.net/)";
constexpr char hidapi[] = "HIDAPI library, version 0.9.0 (https://github.com/libusb/hidapi)";
constexpr char qxlsx[] = "QXlsx library, version 1.3.56 (https://qtexcel.github.io/QXlsx/)";
constexpr char limereport[] = "LimeReport, version 1.5.1 (http://limereport.ru/)";
constexpr char icons[] = "Icons are based on the iconset by 0melapics / Freepik";
constexpr char fonts[] = "Font used: FlorenceSans by ShyFoundry (http://www.shyfoundry.com)";
constexpr char prepare[] = "Подготовка \nокружения\n...\n";
}

class SplashScreen : public QSplashScreen
{
    Q_OBJECT
public:
    SplashScreen(const QPixmap &pixmap = QPixmap(":images/surgery.png"), Qt::WindowFlags f = Qt::WindowFlags());

protected:
    void drawContents(QPainter *painter) override;
};
