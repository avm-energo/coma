#include "sysjournal.h"

namespace journals
{

const QMap<quint32, QString> SysJournal::desriptions = {
    { 1, "Рестарт модуля" },                                                         //
    { 2, "Произошла запись и переход на новую конфигурацию" },                       //
    { 3, "Произошла запись и переход на новую версию ВПО" },                         //
    { 4, "Произошла запись в блок Bhb (hidden block)(базовая)" },                    //
    { 5, "Произошла запись в блок Bhbm (hidden block)(мезонин)" },                   //
    { 6, "Произошёл отказ внешней Flash-памяти 4 Мбайт на базовой плате" },          //
    { 7, "Произошёл отказ внешней Flash-памяти 512 байт на мезонинной плате" },      //
    { 8, "Ошибка работы внешней FRAM памяти" },                                      //
    { 9, "Произошёл отказ при обновлении конфигурации" },                            //
    { 10, "Ошибка загрузки конфигурации из flash памяти" },                          //
    { 11, "Ошибка регулировочных коэффициентов (базовая)" },                         //
    { 12, "Ошибка регулировочных коэффициентов (мезонин)" },                         //
    { 13, "Ошибка перегрев модуля" },                                                //
    { 14, "Напряжение батарейки низкое (< 2,5 В)" },                                 //
    { 15, "Ошибка связи с NTP сервером" },                                           //
    { 16, "Ошибка связи с 1PPS от антенны" },                                        //
    { 17, "Ошибка АЦП (мезонин)" },                                                  //
    { 18, "Ошибка АЦП (базовая)" },                                                  //
    { 19, "Произошла запись регулировочных коэффициентов (базовая)" },               //
    { 20, "Произошла запись регулировочных коэффициентов (мезонин)" },               //
    { 21, "Произошло стирание системного журнала" },                                 //
    { 22, "Произошло стирание рабочего журнала" },                                   //
    { 23, "Произошло стирание осциллограмм" },                                       //
    { 24, "Произошло стирание журнала измерений" },                                  //
    { 25, "Ошибка ВПО" },                                                            //
    { 26, "Ошибка встроенного АЦП 1" },                                              //
    { 27, "Произошла запись в блок Bhb (hidden block)(базовая)" },                   //
    { 28, "Произошла запись в блок Bhbm (hidden block)(мезонин)" },                  //
    { 29, "Ошибка встроенного АЦП 2" },                                              //
    { 30, "Ошибка RTC" },                                                            //
    { 31, "Закрытие сокета 104" },                                                   //
    { 32, "Произошла запись в блок Bhbd (hidden block)(доп)" },                      //
    { 33, "Произошёл отказ внешней Flash-памяти 512 байт на дополнительной плате" }, //
    { 34, "Ошибка регулировочных коэффициентов (доп)" },                             //
    { 35, "Ошибка АЦП (доп)" },                                                      //
    { 36, "Произошла запись регулировочные коэффициенты (доп)" },                    //
    { 37, "Произошла запись в блок Bhbd (hidden block)(доп)" }                       //
};

SysJournal::SysJournal(QObject *parent) : BaseJournal(parent), parser(new EventParser(this))
{
    jourName = "Системный журнал";
    viewName = "system";
    headers = AVM::eventJourHeaders;
    setUserTimezone(headers);
}

void SysJournal::fillTable(const QByteArray &ba)
{
    parser->update(ba);
    const auto data = parser->parse(desriptions, timezone);
    dataModel->fillModel(data);
}

}
