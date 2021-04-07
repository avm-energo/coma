Описание S2files

Используется для отображения конфигурации.

## s2files

```xml
<s2files>
	<record></record>
</s2files>
```

Корневая нода s2files может n элементов record. 

### Record

```xml
<record>
	<id></id>
    <type></type>
    <widget></widget>
</record>
```

Каждая record должна содержать id и type.

* Элемент id обозначает идентификатор элемента S2
* Элемент type указывает на тип элемента (или тип под который элемент маскируется, т.е. char[8] а используется как struct, где sizeof(struct) ==8)

* Элемент widget обозначает виджет, с помощью которого элемент отображается. Если данный элемент отсутствует то соответствующий элемент S2 и  не отображается.

#### Widget

```xml
<widget class='?'>
	<type></type>
	<group></group>
    <parent></parent>
    <field></field>
    <string></string>
    <string-array></string-array>
    <count></count>
    <min></min>
    <max></max>
    <decimals></decimals>
</widget>
```

Элемент widget имеет сложную составную структуру. Он обязательно должен иметь дочерний элемент type и group. Type указает на то, в каком виджете следует отображать соответствующий элемент S2. Group относит элемент к какой-то группе элементов, в соответствии с группой виджет помещается в соответствующую вкладку на экране. Все другие дочерние элементы а также атрибуты зависят от элемента type. 

Если виджет должен иметь описание то для этого используется элемент string, если виджет может иметь в своем составе выбор из списка или множество других виджетов с описанием, то используется string-array в качестве массива строк для описания списка. Например string-array используется для QComboBox в качестве значений выпадающего списка или CheckBoxGroup - для описания каждого входящего в него QCheckBox'a.

Элемент count используется для групп виджетов (DoubleSpinBoxGroup, CheckBoxGroup...) для обозначения количества дочерних виджетов (QDoubleSpinBox, QCheckBox).

Элементы min, max, decimals используются для QDoubleSpinBox и виджетов, имеющих их в составе (DoubleSpinBoxGroup).

* min - минимальное значение спин бокса
* max - максимальное
* decimals - количество точек после запятой

Элемент field специфичен для QComboBox. При стандартном поведении мы читаем/пишем индекс элемента из QComboBox (field=index, опускается т.к. поведение по умолчанию). Если же мы хотим получить иное поведение, т.к. пишем/читаем значения из QComboBox'a напрямую то необходимо добавить элемент 

```xml
<field>data</field>
```

Отдельно рассматривается случай, если какой-то элемент не отображается самостоятельно а является частью другого большого виджета, например несколько элементов отображаются внутри QTableView (пример элементы 1061-1064. Тогда элемент type должен содержать тот типа того класса, внутри которого будут отображаться все элементы группы (например QTableView), а в атрибуте class необходимо указать виджет, отвечающий за представление элемента внутри большого виджета. В этом случае также используется поле parent в качестве ключа, по которому можно найти все связанные элементы.