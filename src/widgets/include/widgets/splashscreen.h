#pragma once

#include <QSplashScreen>

// Сделать translatable
namespace Prog
{
constexpr char name_ru[] = "АВМ-Сервис";
constexpr char department[] = "ООО \"АВМ-Энерго\", 2016-2024";
constexpr char modules[] = "Modules used:";
constexpr auto liblzma = "LZMA library from XZ Utils, version 5.2.5 (https://tukaani.org/xz/)";
constexpr auto hidapi = "HIDAPI library, version 0.10.1 (https://github.com/libusb/hidapi)";
constexpr auto qxlsx = "QXlsx library, version 1.4.4 (https://qtexcel.github.io/QXlsx/)";
constexpr auto limereport = "LimeReport library, version 1.6.8 (https://limereport.ru/)";
constexpr auto qcustomplot = "QCustomPlot library, version 2.1.1 (https://www.qcustomplot.com/)";
constexpr auto icons = "Icons are based on the iconset by 0melapics / Freepik";
constexpr auto fonts = "Font used: FlorenceSans by ShyFoundry (https://shyfoundry.com/home)";
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
