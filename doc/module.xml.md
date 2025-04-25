# Module XML description
## General tags
## Resources
### Alarms
### Config
### Hidden data
### IEC 60870-5-104
### Journals
### Modbus
### Protocom
### Section-tabs
### Sections
#### Section
##### SGroup
###### MWidget
Тег mwidget определяет внешний вид отображаемых данных и привязывает его к конкретному номеру сигнала из списка signals
Свойства тега:
* desc - строка вывода в теле виджета
* view - формат вывода, может принимать значения:
    * float - стандартный вывод для чисел общего вида
    * bitset - вывод битовых полей в виде индикаторов
    * SinglePoint - вывод каждого сигнала в виде отдельного индикатора
    * LineEdit - 
    * Version - 
    * command - кнопка
    * commandvalue - вывод поля для ввода значения рядом с кнопкой
Параметры тега
* start-addr - начальный номер регистра из заданных в разделе signals
* toolTip - подсказка при наведении на поле вывода значения
* count - количество сигналов, выводимых виджетом