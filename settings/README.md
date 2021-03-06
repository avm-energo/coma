

***Описание модуля***


# Modules

```xml
<modules>
    <module mtypeb='?' mtypem='?'></module>
</modules>
```

Корневой элемент modules может содержать неограниченное число дочерних элементов module.

## Module

В атрибутах элементах module обязательно должны быть указаны тип базовой платы и тип мезонинной платы.

```xml
<module>
    <name></name>
    <version></version>
    <resources></resources>
</module>
```

Элемент name отвечает за имя модуля, оно используется для отображения, обязательный элемент. Элемент version отвечает за минимальную подходящую версия ВПО модуля для данной конфигурации. Таким образом внутри ноды modules может содержаться множество отличающихся друг от друга module. Планируется использовать для поддержки множества версий ВПО одновременно.

### Resources

```xml
<resources>
	<alarm name='?'></alarm>
    <journal name='?'></journal>
	<modbus></modbus>
    <protocom></protocom>
    <iec60870></iec60870>
    <s2files></s2files>    
</resources>
```

Нода resources может содержать:

* Ноду alarm - отвечает за сигнализацию.

  Элемент alarm обязательно должен иметь атрибут name.

* Ноду journal - отвечает за журналы.

  Элемент journal обязательно должен иметь атрибут name.

* Ноду modbus - описывает протокол Modbus RTU.

* Ноду protocom - описывает протокол Протоком.

* Ноду iec60870 - описывает протокол МЭК 60870-5-104.

* Ноду s2files - описывает конфигурацию модуля и всё с ней связанное.

### 1. Alarm

```xml
<alarm name>
	<string-array name=alarm::name"+"Events></string-array>
    <quint32 name=alarm::name+"Flags"></quint32>
    <quint32 name ="StartAddr"></quint32>
    <color></color>
</alarm>
```

* Элемент string-array рассматривается как массив строк, должен обязательно содержать атрибут name, состоящий из атрибута name родителя и "Events" - отвечает за описание событий сигнализации.
* Элемент color - отвечает за цвет события по сигнализации, HEX формат.

Также сигнализация должна содержать элемент quint32 с атрибутом name="StartAddr" - он отвечает за стартовый регистр сигнализации.

Сигнализация может содержать элемент quint32 или quint64 в зависимости от размера содержимого с атрибутом, состоящим из атрибута name родительской ноды и "Flags" - этот элемент отвечает за флаги сигнализации.

### 2. Journal

```xml
<journal name>
	<quint32 name=journal::name+"JourId"></quint32>
    <string-array name=journal::name+"JourDescription"></string-array>
</journal>
```

* Элемент quint32 должен содержать атрибут name заданной маски, он отвечает за идентификатор журнала.
* Элемент string-array также должен содержать атрибут name заданной маски, рассматривается как массив строк, описывающий события журнала.

### 3. Modbus 

Данный элемент описывает использование протокола Modbus RTU. Modbus RTU подразумевает использование регистров строго заданного размера - INT16. Поэтому для представления чисел большей длины используются группы регистров (чаще пары: uint32, int32, float). 

```xml
<modbus>
	<group id='?'></group>
    <register id='?'></register>
</modbus>
```

Регистры в устройствах сгруппированы для соответствия между протоколами поэтому здесь они группируются. Но имеется возможность добавить и отдельные регистры без группы, но крайне не рекомендуется. Описание элементов group и register отличается в зависимости от того используются ли input registers или holding registers (read only/ read-write).

Если используются holding регистры, то атрибут id для group или register должен должен содержать вначале литерала r (read holding) или w (write holding). Так как input регистры работают только на чтение то их это правило не касается.

#### 3.1 Group

```xml
<group>
	<function></function>
    <type></type>
    <start-addr></start-addr>
    <count></count>
</group>
```

* Элемент function - функциональный код
* Элемент type - конечный тип получаемого значения
* Элемент start-addr - адрес первого регистра в группе
* Элемент count - количество регистров в группе

#### 3.2 Register

Всё аналогично Group, исключая параметр count. Элемент start-addr становится адресом запрашиваемого регистра.

### 4. Protocom - Протоком

В файле описываются только те части Протокома, которые дублируются по другим интерфейсам. Поэтому Протоком описан так чтобы он мог "прикинуться" другим интерфейсом. 

```xml
<group id='?'>
	<start-addr></start-addr>
    <count></count>
    <block></block>
</group>
```

group может иметь атрибут id. Список обязательных: startValue, alarmModule, to be continued...

* Элемент start-addr указывает на стартовый адрес регистра внутри этой группы по протоколу МЭК 104.

* Элемент count указывает на количество элементов внутри группы. 

  ```c++
  сount * sizeof(group::value_type) == sizeof (block::value_type)
  ```

* Элемент block содержит номер блока, который соответствует данной группе.



### 5. S2Files

Данный элемент отвечает за отображение конфигурацию и значения по умолчанию для конфигурации.

```xml
<s2files>
	<record></record>	
</s2files>
```

Может содержать неограниченное число элементов record, Каждый record соответствует одному элементу в конфигурации. Из всех элементов record формируется список необходимых для отображения элементов. Т.е. если мы опишем элементы 1, 2, 3 а получим элементы 1, 2, 3, 4 то отображаться будут только элемент 1, 2, 3. Другой вариант что указаны элемент 1, 2, 3 а получаем только 1 и 3. Тогда для элемента 2 будет отображаться пустое значение а запишется 0 либо значение по умолчанию для типа этого элемента (default-constructed value).

```xml
<record>
	<id></id>
    <defaultValue></defaultValue>
</record>
```

* Элемент id содержит идентификатор, необходим для формирования списка отображаемых элементов
* Элемент defaultValue содержит значение по умолчанию для данного элемента S2, не обязателен, если отсутствует то считается равным 0 или  значению по умолчанию для типа этого элемента.