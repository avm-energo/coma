#include "checkktf.h"
#include <QLayout>
#include <QGroupBox>
#include "../config/config.h"
#include "../gen/modulebsi.h"
#include "../gen/colors.h"
#include "../widgets/wd_func.h"

CheckKTF::CheckKTF()
{
    //setAttribute(Qt::WA_DeleteOnClose);
    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
                "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: "+QString(UCONFCLR)+";}";

}

CheckKTF::~CheckKTF()
{
}

QWidget *CheckKTF::Bd1W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[4] = {"ф. A","ф. B","ф. C","ср."};
    QString pphase[4] = {"ф. A","ф. B","ф. C","сум."};
    QString ppphase[4] = {"AB","BC","CA","ср."};

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox("Общие");
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


     glyout->addWidget(WDFunc::NewLBL(parent, "Температура ННТ, °С"), 0,0,1,1);
      glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4500), ValuesFormat,\
                                                                    "Температура ННТ обмотки (расчетная), °С"), 1,0,1,1);
     glyout->addWidget(WDFunc::NewLBL(parent, "Tamb, °С"),0,1,1,1);
      glyout->addWidget(WDFunc::NewLBLT(parent, "",  QString::number(4501), ValuesFormat, \
                                                                     "Температура окружающей среды, °С"),1,1,1,1);
     glyout->addWidget(WDFunc::NewLBL(parent, "Частота, Гц"), 0,2,1,1);
      glyout->addWidget(WDFunc::NewLBLT(parent, "",QString::number(2400), ValuesFormat, \
                                                                     "Частота, Гц"), 1,2,1,1);
     glyout->addWidget(WDFunc::NewLBL(parent, "Температура микроконтроллера, °С"), 0,3,1,1);
      glyout->addWidget(WDFunc::NewLBLT(parent, "",QString::number(101), ValuesFormat, \
                                                                       "Температура микроконтроллера, °С"), 1,3,1,1);

    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

  // ...................................

    gb = new QGroupBox("Электрические параметры");
    gb->setFont(ffont);
    vlyout = new QVBoxLayout;
    glyout = new QGridLayout;
    for (i = 0; i < 4; ++i)
    {
        //QString IndexStr = "[" + QString::number(i) + "]";
        glyout->addWidget(WDFunc::NewLBL(parent, "Ueff "+phase[i]+", кВ"),2,i,1,1);
         glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1000+i), ValuesFormat, \
                                          "Истинные действующие значения сигналов трех фаз и их среднее, кВ"),3,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Ieff "+phase[i]+", А"),4,i,1,1);
         glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1400+i), ValuesFormat, \
                                          "Истинные действующие значения сигналов трех фаз и их среднее, А"),5,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "P "+pphase[i]+", кВт"),6,i,1,1);
         glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2420+i), ValuesFormat, \
                                          "Истинная активная мощность по фазам и суммарная, кВт"),7,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "Q "+pphase[i]+", кВАр"),8,i,1,1);
         glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2424+i), ValuesFormat, \
                                          "Реактивная мощность по кажущейся полной и истинной активной, кВАр"),9,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "S "+pphase[i]+", кВА"),10,i,1,1);
         glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2428+i), ValuesFormat, \
                                          "Кажущаяся полная мощность по эфф. токам и нпарямжениям, кВА"),11,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "CosPhi "+phase[i]+""),12,i,1,1);
         glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2432+i), ValuesFormat, \
                                          "Косинус phi по истинной активной мощности,по фазам и средний "),13,i,1,1);
         glyout->addWidget(WDFunc::NewLBL(parent, "Uлин "+ppphase[i]+", кВ"),14,i,1,1);
          glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1220+i), ValuesFormat, \
                                           "Истинные действующие значения линейных напряжений трех фази их среднее, кВ"),15,i,1,1);

    }


    vlyout->addLayout(glyout);
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    // ......................................

    lyout->addLayout(glyout);
    lyout->addStretch(100);
    w->setLayout(lyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *CheckKTF::Bd2W(QWidget *parent)
{

    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox("Оставшееся время работы до предупреждения");
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);

     glyout->addWidget(WDFunc::NewLBL(parent, "При данной нагрузке, сек"), 0,0,1,1);
      glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4010), ValuesFormat,\
                                                                    "Оставшееся время работы до предупрежде-ния при данной нагрузке, сек"), 1,0,1,1);

      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

   //...................................

      gb = new QGroupBox("Допустимое оставшееся время работы");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;


      glyout->addWidget(WDFunc::NewLBL(parent, "При данной нагрузке, сек"),0,0,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4011), ValuesFormat, \
                                        "Допустимое оставшееся время работы при данной нагрузке, сек"),1,0,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 105%, сек"),0,1,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4012), ValuesFormat, \
                                        "Допустимое оставшееся время работы при нагрузке 105%, сек"),1,1,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 110%, сек"),0,2,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4013), ValuesFormat, \
                                        "Допустимое оставшееся время работы при нагрузке 110%, сек"),1,2,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 115%, сек"),2,0,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4014), ValuesFormat, \
                                        "Допустимое оставшееся время работы при нагрузке 115%, сек"),3,0,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 120%, сек"),2,1,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4015), ValuesFormat, \
                                        "Допустимое оставшееся время работы при нагрузке 120%, сек"),3,1,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 125%, сек"),2,2,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4016), ValuesFormat, \
                                        "Допустимое оставшееся время работы при нагрузке 125%, сек"),3,2,1,1);


      glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 130%, сек"),4,0,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4017), ValuesFormat, \
                                        "Допустимое оставшееся время работы при нагрузке 130%, сек"),5,0,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 135%, сек"),4,1,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4018), ValuesFormat, \
                                        "Допустимое оставшееся время работы при нагрузке 135%, сек"),5,1,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 140%, сек"),4,2,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4019), ValuesFormat, \
                                        "Допустимое оставшееся время работы при нагрузке 140%, сек"),5,2,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 145%, сек"),6,0,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4020), ValuesFormat, \
                                        "Допустимое оставшееся время работы при нагрузке 145%, сек"),7,0,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 150%, сек"),6,1,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4021), ValuesFormat, \
                                        "Допустимое оставшееся время работы при нагрузке 150%, сек"),7,1,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 155%, сек"),6,2,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4022), ValuesFormat, \
                                        "Допустимое оставшееся время работы при нагрузке 155%, сек"),7,2,1,1);

       glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 160%, сек"),8,0,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4023), ValuesFormat, \
                                         "Допустимое оставшееся время работы при нагрузке 160%, сек"),9,0,1,1);
       glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 165%, сек"),8,1,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4024), ValuesFormat, \
                                         "Допустимое оставшееся время работы при нагрузке 165%, сек"),9,1,1,1);
       glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 170%, сек"),8,2,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4025), ValuesFormat, \
                                         "Допустимое оставшееся время работы при нагрузке 170%, сек"),9,2,1,1);
       glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 175%, сек"),10,0,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4026), ValuesFormat, \
                                         "Допустимое оставшееся время работы при нагрузке 175%, сек"),11,0,1,1);
       glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 180%, сек"),10,1,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4027), ValuesFormat, \
                                         "Допустимое оставшееся время работы при нагрузке 180%, сек"),11,1,1,1);
       glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 185%, сек"),10,2,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4028), ValuesFormat, \
                                         "Допустимое оставшееся время работы при нагрузке 185%, сек"),11,2,1,1);

        glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 190%, сек"),12,0,1,1);
         glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4029), ValuesFormat, \
                                          "Допустимое оставшееся время работы при нагрузке 190%, сек"),13,0,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 195%, сек"),12,1,1,1);
         glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4030), ValuesFormat, \
                                          "Допустимое оставшееся время работы при нагрузке 195%, сек"),13,1,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "При нагрузке 200%, сек"),12,2,1,1);
         glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4031), ValuesFormat, \
                                          "Допустимое оставшееся время работы при нагрузке 200%, сек"),13,2,1,1);



         vlyout->addLayout(glyout);
         gb->setLayout(vlyout);
         lyout->addWidget(gb);


         // ......................................

         lyout->addLayout(glyout);
         lyout->addStretch(100);
         w->setLayout(lyout);
         w->setStyleSheet(WidgetFormat);
         return w;

}

QWidget *CheckKTF::Bd3W(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox("Изоляция");
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);

     glyout->addWidget(WDFunc::NewLBL(parent, "Относительная скорость старения изоляции"), 0,0,1,1);
      glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4000), ValuesFormat,\
                                                                    "Относительная скорость старения изоляции"), 1,0,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "Интегральное старение изоляции, час"), 2,0,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4001), ValuesFormat,\
                                                                     "Интегральное старение изоляции, час"), 3,0,1,1);
       glyout->addWidget(WDFunc::NewLBL(parent, "Остаточный ресурс изоляции, час"), 4,0,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(4002), ValuesFormat,\
                                                                      "Остаточный ресурс изоляции, час"), 5,0,1,1);

      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

      // ......................................

      gb = new QGroupBox("Амплитуда пускового тока при последнем пуске");
       gb->setFont(ffont);
       vlyout = new QVBoxLayout;
       glyout = new QGridLayout;

       glyout->addWidget(WDFunc::NewLBL(parent, "Амплитуда, А"), 6,0,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(5200), ValuesFormat,\
                                                                      "Амплитуда пускового тока при последнем пуске, А"), 7,0,1,1);


        vlyout->addLayout(glyout);
        gb->setLayout(vlyout);
        lyout->addWidget(gb);

      // ......................................

      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;


}

QWidget *CheckKTF::Bd4W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[4] = {"ф. A","ф. B","ф. C","ср."};
    QString pphase[4] = {"ф. A","ф. B","ф. C","сум."};
    QString ppphase[4] = {"AB","BC","CA","ср."};

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


      gb = new QGroupBox("Электрические параметры по 1-й гармонике");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;
      for (i = 0; i < 4; ++i)
      {

          glyout->addWidget(WDFunc::NewLBL(parent, "Ueff "+phase[i]+", кВ"),0,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1004+i), ValuesFormat, \
                                            "Действующие значения сигналов трех фаз по 1-й гармонике и их среднее, кВ"),1,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, "Ieff "+phase[i]+", А"),2,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1404+i), ValuesFormat, \
                                            "Действующие значения сигналов трех фаз по 1-й гармонике и их среднее, А"),3,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, "P "+pphase[i]+", кВт"),4,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2450+i), ValuesFormat, \
                                            "Активная мощность по 1-й гармонике, по фазам и суммарная, кВт"),5,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, "Q "+pphase[i]+", кВАр"),6,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2454+i), ValuesFormat, \
                                            "Реактивня мощность по 1-й гармонике, кВАр"),7,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, "S "+pphase[i]+", кВА"),8,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2458+i), ValuesFormat, \
                                            "Полная мощность по 1-й гармонике, кВА"),9,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, "CosPhi "+phase[i]+""),10,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(2462+i), ValuesFormat, \
                                            "Косинус phi по 1-й гармонике,по фазам и средний "),11,i,1,1);
           glyout->addWidget(WDFunc::NewLBL(parent, "Uлин "+ppphase[i]+", кВ"),12,i,1,1);
            glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1224+i), ValuesFormat, \
                                             "Действующие значения 1-й гармоники линейных напряжений трех фази их среднее, кВ"),13,i,1,1);

      }


      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

      // ......................................

      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;


}

QWidget *CheckKTF::Bd5W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = {"ф. A","ф. B","ф. C"};
   // QString pphase[4] = {"ф. A","ф. B","ф. C","сум."};
   // QString ppphase[4] = {"AB","BC","CA","ср."};

    //...................................

    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


      gb = new QGroupBox("Данные по напряжению");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;



      glyout->addWidget(WDFunc::NewLBL(parent, "U0, кВ"), 0,0,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1020), ValuesFormat,\
                                                                     "Напряжение нулевой последовательности гр.1, кВ"), 1,0,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "U1, кВ"),0,1,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "",  QString::number(1021), ValuesFormat, \
                                                                      "Температура окружающей среды, °С"),1,1,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "U2, кВ"), 0,2,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "",QString::number(1022), ValuesFormat, \
                                                                      "Частота, Гц"), 1,2,1,1);
       glyout->addWidget(WDFunc::NewLBL(parent, "KunsimU0"), 2,0,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "",QString::number(1023), ValuesFormat, \
                                                                        "Коэффициент несимметрии напряжения по обратной последовательности гр.1, %"), 3,0,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "KunsimU2"), 2,1,1,1);
         glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1024), ValuesFormat,\
                                                                       "Коэффициент несимметрии напряжения по нулевой последовательности гр.1, %"), 3,1,1,1);
         for (i = 0; i < 3; ++i)
         {


         glyout->addWidget(WDFunc::NewLBL(parent, "THD" +phase[i]+""),4,i,1,1);
          glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1025+i), ValuesFormat, \
                                           "Общий коэффициент гарм. искажений напряжения фазы "+phase[i] +" гр.1"),5,i,1,1);
         }

      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

      // ......................................


      gb = new QGroupBox("Данные по току");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;


      glyout->addWidget(WDFunc::NewLBL(parent, "I0, кВ"), 6,0,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1420), ValuesFormat,\
                                                                     "Ток нулевой последовательности гр.1, A"), 7,0,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "I1, кВ"),6,1,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "",  QString::number(1421), ValuesFormat, \
                                                                      "Ток прямой последовательности гр.1, А"),7,1,1,1);
      glyout->addWidget(WDFunc::NewLBL(parent, "I2, А"), 6,2,1,1);
       glyout->addWidget(WDFunc::NewLBLT(parent, "",QString::number(1422), ValuesFormat, \
                                                                      "Ток обратной последовательности гр.1, А"), 7,2,1,1);
       glyout->addWidget(WDFunc::NewLBL(parent, "KunsimI0"), 8,0,1,1);
        glyout->addWidget(WDFunc::NewLBLT(parent, "",QString::number(1423), ValuesFormat, \
                                                                        "Коэффициент несимметрии тока по обратной последовательности гр.1,%"), 9,0,1,1);
        glyout->addWidget(WDFunc::NewLBL(parent, "KunsimI2"), 8,1,1,1);
         glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1424), ValuesFormat,\
                                                                       "Коэффициент несимметрии тока по нулевой последовательности гр.1,%"), 9,1,1,1);
         for (i = 0; i < 3; ++i)
         {

         glyout->addWidget(WDFunc::NewLBL(parent, "THD" +phase[i]+""),10,i,1,1);
          glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1425+i), ValuesFormat, \
                                           "Общий коэффициент гарм. искажений тока фазы "+phase[i] +" гр.1"),11,i,1,1);
         }

      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);


       // ......................................


      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;


}





QWidget *CheckKTF::Bd6W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = {"ф.A ","ф.B ","ф.C "};
    QString pphase[4] = {"ф. A","ф. B","ф. C","сум."};
    QString ppphase[4] = {"AB","BC","CA","ср."};

    //...................................


    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


      gb = new QGroupBox("Данные по гармоническому составу напряжения");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;
      for (i = 0; i < 3; ++i)
      {

          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 2 "),0,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1028+i*61), ValuesFormat, \
                                            "2-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),1,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 3 "),2,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1029+i*61), ValuesFormat, \
                                             "3-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),3,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 4 "),4,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1030+i*61), ValuesFormat, \
                                             "4-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),5,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent,phase[i]+ "Гм. 5 "),6,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1031+i*61), ValuesFormat, \
                                             "5-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),7,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 6 "),8,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1032+i*61), ValuesFormat, \
                                             "6-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),9,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 7 "),10,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1033+i*61), ValuesFormat, \
                                             "7-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),11,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 8 "),12,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1034+i*61), ValuesFormat, \
                                              "8-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),13,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 9 "),14,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1035+i*61), ValuesFormat, \
                                              "9-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),15,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 10 "),16,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1036+i*61), ValuesFormat, \
                                               "10-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),17,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 11 "),18,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1037+i*61), ValuesFormat, \
                                               "11-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),19,i,1,1);

      }


      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

       // ......................................


      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;


}

QWidget *CheckKTF::Bd7W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = {"ф.A ","ф.B ","ф.C "};

    //...................................


    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


      gb = new QGroupBox("Данные по гармоническому составу напряжения");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;
      for (i = 0; i < 3; ++i)
      {

          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 12 "),0,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1038+i*61), ValuesFormat, \
                                            "12-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),1,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 13 "),2,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1039+i*61), ValuesFormat, \
                                             "13-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),3,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 14 "),4,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1040+i*61), ValuesFormat, \
                                             "14-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),5,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent,phase[i]+ "Гм. 15 "),6,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1041+i*61), ValuesFormat, \
                                             "15-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),7,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 16 "),8,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1042+i*61), ValuesFormat, \
                                             "16-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),9,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 17 "),10,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1043+i*61), ValuesFormat, \
                                             "17-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),11,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 18 "),12,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1044+i*61), ValuesFormat, \
                                              "18-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),13,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 19 "),14,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1045+i*61), ValuesFormat, \
                                              "19-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),15,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 20 "),16,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1046+i*61), ValuesFormat, \
                                               "20-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),17,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 21 "),18,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1047+i*61), ValuesFormat, \
                                               "21-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),19,i,1,1);

      }


      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

       // ......................................


      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;


}

QWidget *CheckKTF::Bd8W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = {"ф.A ","ф.B ","ф.C "};

    //...................................


    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


      gb = new QGroupBox("Данные по гармоническому составу напряжения");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;
      for (i = 0; i < 3; ++i)
      {

          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 22 "),0,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1048+i*61), ValuesFormat, \
                                            "22-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),1,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 23 "),2,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1049+i*61), ValuesFormat, \
                                             "23-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),3,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 24 "),4,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1050+i*61), ValuesFormat, \
                                             "24-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),5,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent,phase[i]+ "Гм. 25 "),6,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1051+i*61), ValuesFormat, \
                                             "25-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),7,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 26 "),8,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1052+i*61), ValuesFormat, \
                                             "26-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),9,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 27 "),10,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1053+i*61), ValuesFormat, \
                                             "27-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),11,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 28 "),12,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1054+i*61), ValuesFormat, \
                                              "28-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),13,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 29 "),14,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1055+i*61), ValuesFormat, \
                                              "29-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),15,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 30 "),16,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1056+i*61), ValuesFormat, \
                                               "30-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),17,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 31 "),18,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1057+i*61), ValuesFormat, \
                                               "31-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),19,i,1,1);

      }


      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

       // ......................................


      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;

}

QWidget *CheckKTF::Bd9W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = {"ф.A ","ф.B ","ф.C "};

    //...................................


    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


      gb = new QGroupBox("Данные по гармоническому составу напряжения");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;
      for (i = 0; i < 3; ++i)
      {

          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 32 "),0,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1058+i*61), ValuesFormat, \
                                            "32-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),1,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 33 "),2,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1059+i*61), ValuesFormat, \
                                             "33-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),3,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 34 "),4,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1060+i*61), ValuesFormat, \
                                             "34-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),5,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent,phase[i]+ "Гм. 35 "),6,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1061+i*61), ValuesFormat, \
                                             "35-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),7,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 36 "),8,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1062+i*61), ValuesFormat, \
                                             "36-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),9,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 37 "),10,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1063+i*61), ValuesFormat, \
                                             "37-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),11,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 38 "),12,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1064+i*61), ValuesFormat, \
                                              "38-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),13,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 39 "),14,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1065+i*61), ValuesFormat, \
                                              "39-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),15,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 40 "),16,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1066+i*61), ValuesFormat, \
                                               "40-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),17,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 41 "),18,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1067+i*61), ValuesFormat, \
                                               "41-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),19,i,1,1);

      }


      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

       // ......................................


      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;


}

QWidget *CheckKTF::Bd10W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = {"ф.A ","ф.B ","ф.C "};

    //...................................


    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


      gb = new QGroupBox("Данные по гармоническому составу напряжения");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;
      for (i = 0; i < 3; ++i)
      {

          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 42 "),0,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1068+i*61), ValuesFormat, \
                                            "42-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),1,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 43 "),2,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1069+i*61), ValuesFormat, \
                                             "43-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),3,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 44 "),4,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1070+i*61), ValuesFormat, \
                                             "44-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),5,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent,phase[i]+ "Гм. 45 "),6,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1071+i*61), ValuesFormat, \
                                             "45-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),7,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 46 "),8,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1072+i*61), ValuesFormat, \
                                             "46-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),9,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 47 "),10,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1073+i*61), ValuesFormat, \
                                             "47-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),11,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 48 "),12,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1074+i*61), ValuesFormat, \
                                              "48-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),13,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 49 "),14,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1075+i*61), ValuesFormat, \
                                              "49-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),15,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 50 "),16,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1076+i*61), ValuesFormat, \
                                               "50-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),17,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 51 "),18,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1077+i*61), ValuesFormat, \
                                               "51-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),19,i,1,1);

      }


      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

       // ......................................


      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;


}

QWidget *CheckKTF::Bd11W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = {"ф.A ","ф.B ","ф.C "};

    //...................................


    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


      gb = new QGroupBox("Данные по гармоническому составу напряжения");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;
      for (i = 0; i < 3; ++i)
      {

          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 52 "),0,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1078+i*61), ValuesFormat, \
                                            "52-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),1,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 53 "),2,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1079+i*61), ValuesFormat, \
                                             "53-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),3,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 54 "),4,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1080+i*61), ValuesFormat, \
                                             "54-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),5,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent,phase[i]+ "Гм. 55 "),6,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1081+i*61), ValuesFormat, \
                                             "55-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),7,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 56 "),8,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1082+i*61), ValuesFormat, \
                                             "56-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),9,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 57 "),10,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1083+i*61), ValuesFormat, \
                                             "57-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),11,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 58 "),12,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1084+i*61), ValuesFormat, \
                                              "58-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),13,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 59 "),14,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1085+i*61), ValuesFormat, \
                                              "59-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),15,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 60 "),16,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1086+i*61), ValuesFormat, \
                                               "60-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),17,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 61 "),18,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1087+i*61), ValuesFormat, \
                                               "61-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),19,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 62 "),20,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1088+i*61), ValuesFormat, \
                                                "62-я гармоника напряжения в фазе" +phase[i] +"в % от 1-й",true),21,i,1,1);

      }


      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

       // ......................................


      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;


}






QWidget *CheckKTF::Bd12W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = {"ф.A ","ф.B ","ф.C "};

    //...................................


    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


      gb = new QGroupBox("Данные по гармоническому составу тока");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;
      for (i = 0; i < 3; ++i)
      {

          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 2 "),0,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1428+i*61), ValuesFormat, \
                                            "2-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),1,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 3 "),2,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1429+i*61), ValuesFormat, \
                                             "3-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),3,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 4 "),4,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1430+i*61), ValuesFormat, \
                                             "4-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),5,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent,phase[i]+ "Гм. 5 "),6,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1431+i*61), ValuesFormat, \
                                             "5-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),7,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 6 "),8,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1432+i*61), ValuesFormat, \
                                             "6-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),9,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 7 "),10,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1433+i*61), ValuesFormat, \
                                             "7-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),11,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 8 "),12,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1434+i*61), ValuesFormat, \
                                              "8-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),13,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 9 "),14,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1435+i*61), ValuesFormat, \
                                              "9-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),15,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 10 "),16,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1436+i*61), ValuesFormat, \
                                               "10-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),17,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 11 "),18,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1437+i*61), ValuesFormat, \
                                               "11-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),19,i,1,1);
      }


      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

       // ......................................


      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;


}

QWidget *CheckKTF::Bd13W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = {"ф.A ","ф.B ","ф.C "};

    //...................................


    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


      gb = new QGroupBox("Данные по гармоническому составу тока");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;
      for (i = 0; i < 3; ++i)
      {

          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 12 "),0,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1438+i*61), ValuesFormat, \
                                            "12-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),1,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 13 "),2,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1439+i*61), ValuesFormat, \
                                             "13-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),3,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 14 "),4,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1440+i*61), ValuesFormat, \
                                             "14-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),5,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent,phase[i]+ "Гм. 15 "),6,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1441+i*61), ValuesFormat, \
                                             "15-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),7,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 16 "),8,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1442+i*61), ValuesFormat, \
                                             "16-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),9,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 17 "),10,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1443+i*61), ValuesFormat, \
                                             "17-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),11,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 18 "),12,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1444+i*61), ValuesFormat, \
                                              "18-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),13,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 19 "),14,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1445+i*61), ValuesFormat, \
                                              "19-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),15,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 20 "),16,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1446+i*61), ValuesFormat, \
                                               "20-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),17,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 21 "),18,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1447+i*61), ValuesFormat, \
                                               "21-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),19,i,1,1);
      }

      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

       // ......................................


      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;


}

QWidget *CheckKTF::Bd14W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = {"ф.A ","ф.B ","ф.C "};

    //...................................


    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


      gb = new QGroupBox("Данные по гармоническому составу тока");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;
      for (i = 0; i < 3; ++i)
      {

          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 22 "),0,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1448+i*61), ValuesFormat, \
                                            "22-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),1,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 23 "),2,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1449+i*61), ValuesFormat, \
                                             "23-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),3,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 24 "),4,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1450+i*61), ValuesFormat, \
                                             "24-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),5,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent,phase[i]+ "Гм. 25 "),6,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1451+i*61), ValuesFormat, \
                                             "25-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),7,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 26 "),8,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1452+i*61), ValuesFormat, \
                                             "26-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),9,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 27 "),10,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1453+i*61), ValuesFormat, \
                                             "27-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),11,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 28 "),12,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1454+i*61), ValuesFormat, \
                                              "28-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),13,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 29 "),14,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1455+i*61), ValuesFormat, \
                                              "29-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),15,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 30 "),16,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1456+i*61), ValuesFormat, \
                                               "30-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),17,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 31 "),18,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1457+i*61), ValuesFormat, \
                                               "31-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),19,i,1,1);
      }

      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

       // ......................................


      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;


}

QWidget *CheckKTF::Bd15W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = {"ф.A ","ф.B ","ф.C "};

    //...................................


    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


      gb = new QGroupBox("Данные по гармоническому составу тока");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;
      for (i = 0; i < 3; ++i)
      {

          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 32 "),0,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1458+i*61), ValuesFormat, \
                                            "32-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),1,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 33 "),2,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1459+i*61), ValuesFormat, \
                                             "33-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),3,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 34 "),4,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1460+i*61), ValuesFormat, \
                                             "34-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),5,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent,phase[i]+ "Гм. 35 "),6,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1461+i*61), ValuesFormat, \
                                             "35-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),7,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 36 "),8,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1462+i*61), ValuesFormat, \
                                             "36-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),9,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 37 "),10,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1463+i*61), ValuesFormat, \
                                             "37-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),11,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 38 "),12,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1464+i*61), ValuesFormat, \
                                              "38-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),13,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 39 "),14,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1465+i*61), ValuesFormat, \
                                              "39-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),15,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 40 "),16,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1466+i*61), ValuesFormat, \
                                               "40-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),17,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 41 "),18,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1467+i*61), ValuesFormat, \
                                               "41-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),19,i,1,1);
      }

      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

       // ......................................


      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;


}

QWidget *CheckKTF::Bd16W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = {"ф.A ","ф.B ","ф.C "};

    //...................................


    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


      gb = new QGroupBox("Данные по гармоническому составу тока");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;
      for (i = 0; i < 3; ++i)
      {

          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 42 "),0,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1468+i*61), ValuesFormat, \
                                            "42-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),1,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 43 "),2,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1469+i*61), ValuesFormat, \
                                             "43-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),3,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 44 "),4,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1470+i*61), ValuesFormat, \
                                             "44-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),5,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent,phase[i]+ "Гм. 45 "),6,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1471+i*61), ValuesFormat, \
                                             "45-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),7,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 46 "),8,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1472+i*61), ValuesFormat, \
                                             "46-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),9,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 47 "),10,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1473+i*61), ValuesFormat, \
                                             "47-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),11,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 48 "),12,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1474+i*61), ValuesFormat, \
                                              "48-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),13,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 49 "),14,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1475+i*61), ValuesFormat, \
                                              "49-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),15,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 50 "),16,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1476+i*61), ValuesFormat, \
                                               "50-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),17,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 51 "),18,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1477+i*61), ValuesFormat, \
                                               "51-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),19,i,1,1);
      }

      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

       // ......................................


      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;


}


QWidget *CheckKTF::Bd17W(QWidget *parent)
{
    int i;
    QWidget *w = new QWidget(parent);
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString phase[3] = {"ф.A ","ф.B ","ф.C "};

    //...................................


    QFont ffont;
    QGroupBox *gb = new QGroupBox();
    ffont.setFamily("Times");
    ffont.setPointSize(11);
     gb->setFont(ffont);


      gb = new QGroupBox("Данные по гармоническому составу тока");
      gb->setFont(ffont);
      vlyout = new QVBoxLayout;
      glyout = new QGridLayout;
      for (i = 0; i < 3; ++i)
      {

          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 52 "),0,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1478+i*61), ValuesFormat, \
                                            "52-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),1,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 53 "),2,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1479+i*61), ValuesFormat, \
                                             "53-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),3,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 54 "),4,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1480+i*61), ValuesFormat, \
                                             "54-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),5,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent,phase[i]+ "Гм. 55 "),6,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1481+i*61), ValuesFormat, \
                                             "55-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),7,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 56 "),8,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1482+i*61), ValuesFormat, \
                                             "56-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),9,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 57 "),10,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1483+i*61), ValuesFormat, \
                                             "57-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),11,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 58 "),12,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1484+i*61), ValuesFormat, \
                                              "58-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),13,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 59 "),14,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1485+i*61), ValuesFormat, \
                                              "59-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),15,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 60 "),16,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1486+i*61), ValuesFormat, \
                                               "60-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),17,i,1,1);
          glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 61 "),18,i,1,1);
           glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1487+i*61), ValuesFormat, \
                                               "61-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),19,i,1,1);
           glyout->addWidget(WDFunc::NewLBL(parent, phase[i]+"Гм. 62 "),18,i,1,1);
            glyout->addWidget(WDFunc::NewLBLT(parent, "", QString::number(1488+i*61), ValuesFormat, \
                                                "62-я гармоника тока в фазе" +phase[i] +"в % от 1-й",true),19,i,1,1);
      }

      vlyout->addLayout(glyout);
      gb->setLayout(vlyout);
      lyout->addWidget(gb);

       // ......................................


      lyout->addLayout(glyout);
      lyout->addStretch(100);
      w->setLayout(lyout);
      w->setStyleSheet(WidgetFormat);
      return w;


}






void CheckKTF::FillBd(QWidget *parent, QString Name, QString Value)
{
    WDFunc::SetLBLText(parent, Name, Value);
}

QWidget *CheckKTF::BdUI(int bdnum)
{
    Q_UNUSED(bdnum);
    return nullptr;
}

void CheckKTF::SetDefaultValuesToWrite()
{

}

void CheckKTF::PrepareAnalogMeasurements()
{

}

void CheckKTF::ChooseValuesToWrite()
{

}

void CheckKTF::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row);
    Q_UNUSED(bdnum);

}

void CheckKTF::PrepareHeadersForFile(int row)
{
     Q_UNUSED(row);

}

void CheckKTF::RefreshAnalogValues(int bdnum)
{
  Q_UNUSED(bdnum);
}

QWidget *CheckKTF::CustomTab()
{
    QWidget *w = new QWidget;

    return w;
}

void CheckKTF::FillBd13(QWidget *parent)
{
    //WDFunc::SetLBLText(parent, "value0", WDFunc::StringValueWithCheck(Bd_block1.NUM, 3));
    WDFunc::SetLBLText(parent, QString::number(4500), WDFunc::StringValueWithCheck(Bd_block13.TempWinding, 3));
 //   WDFunc::SetLBLText(parent,  QString::number(4500), WDFunc::StringValueWithCheck(Bd_block13.ExtTempWin[0], 3));


 //   if (ModuleBSI::GetMType(BoardTypes::BT_BASE) != Config::MTB_A2)
 //   {
        WDFunc::SetLBLText(parent,  QString::number(4501), WDFunc::StringValueWithCheck(Bd_block13.temperature, 3));
 //   }

}

void CheckKTF::FillBd0(QWidget *parent)
{
   WDFunc::SetLBLText(parent,  QString::number(101), WDFunc::StringValueWithCheck(Bd_block0.Temp, 3));
}


void CheckKTF::FillBd17(QWidget *parent)
{
   WDFunc::SetLBLText(parent,  QString::number(2400), WDFunc::StringValueWithCheck(Bd_block17.Frequency, 3));
}

void CheckKTF::FillBd10(QWidget *parent)
{
  for (int i = 0; i < 4; i++)
  {
      WDFunc::SetLBLText(parent, QString::number(2420+i), WDFunc::StringValueWithCheck(Bd_block10.PNatf[i], 3));
      WDFunc::SetLBLText(parent, QString::number(2424+i), WDFunc::StringValueWithCheck(Bd_block10.QNatf[i], 3));
      WDFunc::SetLBLText(parent, QString::number(2428+i), WDFunc::StringValueWithCheck(Bd_block10.SNatf[i], 3));
      WDFunc::SetLBLText(parent, QString::number(2432+i), WDFunc::StringValueWithCheck(Bd_block10.CosPhiNat[i], 3));


      WDFunc::SetLBLText(parent, QString::number(2450+i), WDFunc::StringValueWithCheck(Bd_block10.Pf[i], 3));
      WDFunc::SetLBLText(parent, QString::number(2454+i), WDFunc::StringValueWithCheck(Bd_block10.Qf[i], 3));
      WDFunc::SetLBLText(parent, QString::number(2458+i), WDFunc::StringValueWithCheck(Bd_block10.Sf[i], 3));
      WDFunc::SetLBLText(parent, QString::number(2462+i), WDFunc::StringValueWithCheck(Bd_block10.CosPhi, 3));

  }
}


void CheckKTF::FillBd2(QWidget *parent)
{

  for (int i = 0; i < 4; i++)
  {
    WDFunc::SetLBLText(parent, QString::number(1000+i), WDFunc::StringValueWithCheck(Bd_block2.IUefNat_filt[i], 3));

    WDFunc::SetLBLText(parent, QString::number(1004+i), WDFunc::StringValueWithCheck(Bd_block2.IUeff_filtered[i], 3));

  }

    WDFunc::SetLBLText(parent, QString::number(1020), WDFunc::StringValueWithCheck(Bd_block2.U0, 3));
     WDFunc::SetLBLText(parent, QString::number(1021), WDFunc::StringValueWithCheck(Bd_block2.U1, 3));
      WDFunc::SetLBLText(parent, QString::number(1022), WDFunc::StringValueWithCheck(Bd_block2.U2, 3));
       WDFunc::SetLBLText(parent, QString::number(1023), WDFunc::StringValueWithCheck(Bd_block2.Kunsim0, 3));
        WDFunc::SetLBLText(parent, QString::number(1024), WDFunc::StringValueWithCheck(Bd_block2.Kunsim2, 3));


 for (int i = 0; i < 4; i++)
     {
        WDFunc::SetLBLText(parent, QString::number(1025+i), WDFunc::StringValueWithCheck(Bd_block2.THD[i], 3));
    // WDFunc::SetLBLText(parent, QString::number(1025+i), WDFunc::StringValueWithCheck(QQQ, 3));

      }
//QQQ=QQQ+1;

 for (int j = 0; j < 3; j++)
 {
    for (int i = 0; i < 61; i++)
      {

  WDFunc::SetLBLText(parent, QString::number(1028+i+61*j), WDFunc::StringValueWithCheck(Bd_block2.UI_Harm[j][i], 3));
        //  WDFunc::SetLBLText(parent, QString::number(1028+i+61*j), WDFunc::StringValueWithCheck(QQQ, 3));

    }
 }

//QQQ=QQQ+1;

}

void CheckKTF::FillBd3 (QWidget *parent)
{
  for (int i = 0; i < 4; i++)
  {
     WDFunc::SetLBLText(parent, QString::number(1400+i), WDFunc::StringValueWithCheck(Bd_block3.IUefNat_filt[i], 3));

     WDFunc::SetLBLText(parent, QString::number(1404+i), WDFunc::StringValueWithCheck(Bd_block3.IUeff_filtered[i], 3));
  }

  WDFunc::SetLBLText(parent, QString::number(1420), WDFunc::StringValueWithCheck(Bd_block2.U0, 3));
   WDFunc::SetLBLText(parent, QString::number(1421), WDFunc::StringValueWithCheck(Bd_block2.U1, 3));
    WDFunc::SetLBLText(parent, QString::number(1422), WDFunc::StringValueWithCheck(Bd_block2.U2, 3));
     WDFunc::SetLBLText(parent, QString::number(1423), WDFunc::StringValueWithCheck(Bd_block2.Kunsim0, 3));
      WDFunc::SetLBLText(parent, QString::number(1424), WDFunc::StringValueWithCheck(Bd_block2.Kunsim2, 3));


for (int i = 0; i < 4; i++)
   {
      WDFunc::SetLBLText(parent, QString::number(1425+i), WDFunc::StringValueWithCheck(Bd_block2.THD[i], 3));

    }


for (int j = 0; j < 3; j++)
{
   for (int i = 0; i < 61; i++)
     {

  WDFunc::SetLBLText(parent, QString::number(1428+i+61*j), WDFunc::StringValueWithCheck(Bd_block3.UI_Harm[j][i], 3));

   }
}




}

void CheckKTF::FillBd8 (QWidget *parent)
{
  for (int j = 0; j < 4; j++)
  {
      WDFunc::SetLBLText(parent, QString::number(1220+j), WDFunc::StringValueWithCheck(Bd_block8.UefNatLin_filt[j], 3));

      WDFunc::SetLBLText(parent, QString::number(1224+j), WDFunc::StringValueWithCheck(Bd_block8.UeffLin_filtered[j], 3));

  }
}

void CheckKTF::FillBd11 (QWidget *parent)
{

      WDFunc::SetLBLText(parent, QString::number(4010), WDFunc::StringValueWithCheck(Bd_block11.Tpred, 3));
      WDFunc::SetLBLText(parent, QString::number(4011), WDFunc::StringValueWithCheck(Bd_block11.Tdop, 3));
      WDFunc::SetLBLText(parent, QString::number(4012), WDFunc::StringValueWithCheck(Bd_block11.Tdop105, 3));
      WDFunc::SetLBLText(parent, QString::number(4013), WDFunc::StringValueWithCheck(Bd_block11.Tdop110, 3));
      WDFunc::SetLBLText(parent, QString::number(4014), WDFunc::StringValueWithCheck(Bd_block11.Tdop115, 3));
      WDFunc::SetLBLText(parent, QString::number(4015), WDFunc::StringValueWithCheck(Bd_block11.Tdop120, 3));
      WDFunc::SetLBLText(parent, QString::number(4016), WDFunc::StringValueWithCheck(Bd_block11.Tdop125, 3));
      WDFunc::SetLBLText(parent, QString::number(4017), WDFunc::StringValueWithCheck(Bd_block11.Tdop130, 3));
      WDFunc::SetLBLText(parent, QString::number(4018), WDFunc::StringValueWithCheck(Bd_block11.Tdop135, 3));
      WDFunc::SetLBLText(parent, QString::number(4019), WDFunc::StringValueWithCheck(Bd_block11.Tdop140, 3));
      WDFunc::SetLBLText(parent, QString::number(4020), WDFunc::StringValueWithCheck(Bd_block11.Tdop145, 3));
      WDFunc::SetLBLText(parent, QString::number(4021), WDFunc::StringValueWithCheck(Bd_block11.Tdop150, 3));
      WDFunc::SetLBLText(parent, QString::number(4022), WDFunc::StringValueWithCheck(Bd_block11.Tdop155, 3));
      WDFunc::SetLBLText(parent, QString::number(4023), WDFunc::StringValueWithCheck(Bd_block11.Tdop160, 3));
      WDFunc::SetLBLText(parent, QString::number(4024), WDFunc::StringValueWithCheck(Bd_block11.Tdop165, 3));
      WDFunc::SetLBLText(parent, QString::number(4025), WDFunc::StringValueWithCheck(Bd_block11.Tdop170, 3));
      WDFunc::SetLBLText(parent, QString::number(4026), WDFunc::StringValueWithCheck(Bd_block11.Tdop175, 3));
      WDFunc::SetLBLText(parent, QString::number(4027), WDFunc::StringValueWithCheck(Bd_block11.Tdop180, 3));
      WDFunc::SetLBLText(parent, QString::number(4028), WDFunc::StringValueWithCheck(Bd_block11.Tdop185, 3));
      WDFunc::SetLBLText(parent, QString::number(4029), WDFunc::StringValueWithCheck(Bd_block11.Tdop190, 3));
      WDFunc::SetLBLText(parent, QString::number(4030), WDFunc::StringValueWithCheck(Bd_block11.Tdop195, 3));
      WDFunc::SetLBLText(parent, QString::number(4031), WDFunc::StringValueWithCheck(Bd_block11.Tdop200, 3));


}

void CheckKTF::FillBd9 (QWidget *parent)
{
      WDFunc::SetLBLText(parent, QString::number(4000), WDFunc::StringValueWithCheck(Bd_block9.Vst, 3));
      WDFunc::SetLBLText(parent, QString::number(4001), WDFunc::StringValueWithCheck(Bd_block9.Age, 3));
      WDFunc::SetLBLText(parent, QString::number(4002), WDFunc::StringValueWithCheck(Bd_block9.Resurs, 3));

}


void CheckKTF::FillBd18 (QWidget *parent)
{
      WDFunc::SetLBLText(parent, QString::number(5200), WDFunc::StringValueWithCheck(Bd_block18.Istart, 3));

}













