# Ръководство за работа с устройството
## Части и елементи
- пулт за управление с бутон и два светодиода - зелен и червен. Използва се платка Arduino Pro Micro (https://www.sparkfun.com/products/12640) с MCU **ATmega32U4** 5V/16MHz.
- кабел с дължина 4 метра
- ултразвуков сензор до 5 метра (https://www.dfrobot.com/product-1863.html)

## Функции
Устройството засича наличието на обект пред сензора и подава през USB кабела команда <kbd>Volume UP</kbd>. Разпозава се от телефон или компютър като HID устройство (клавиатура). Ако обект бъде засечен, следващото измерване и команда <kbd>Volume UP</kbd>, се извършва най-рано след 1 сек т.е. измерването е блокирано за 1 секунда. 

Командата <kbd>Volume UP</kbd> се използва от съответната апликация, която "цъква" време, ако получи <kbd>Volume UP</kbd>.

**Максимална дистанция на измерване - 5 метра.**

За да работи с Андроид е необходимо устройството да подържа **USB OTG** и **Host mode**!

## Основна работа с устройството
- Сензорът се поставя на височината на засичания обект, на мислената линия на която ще се мери времето.
- USB кабелът се закача към телефон или таблет с USB OTG адаптер.
- След първоначано включване червеният светодиод мига бързо 5 секунди.
- След 5 секунди зеленият светодиод мига няколко пъти в зависимост от настройката за чувствителност на засичане, която е записана перманентно в паметта на устройството. Светодиодът светва за 1 секунда, с интервал между светванията 0.5 сек. Броят на светванията показва чувствителността по следната *Таблица 1*:

Брой премигвания | Чувствителност
:---: | :---:
1 | 1 метър
2 | 2 метра
3 | 3 метра
4 | 4 метра
5 | 4.5 метра
6 | 5 метра

- След като завърши процесът с индикиране на чувствителността устройството минава в нормален работен режим (около 10 сек след вклюване към USB). Нормалния режим се индикира от зеления светодиод с кратки премигвания (75 милисекунди), през 1 сек. 
- Ако премине обект пред сензорът червеният светодиод премигва за 0.2 сек. и подава през USB <kbd>Volume UP</kbd>.

## Измерване на време с устройството и работа с бутона
1. Сензорът се поставя на височината на засичания обект, на мислената линия на която ще се мери времето.
2. Закачете Андроид устройството с USB кабел към контролния пулт през USB OTG адаптер или директно с USB OTG кабел. 
3. Изчакайте инициализацията на устройството в първите 10 сек както е описано в **Основна работа с устройството**.
4. След като инициализацията е приключила, устройството вече мери за наличието на обект пред сензора, като индикира зеления светодиод с кратки премигвания (продължителност 75 милисекунди), през 1 сек. **Кратките премигвания показват, че устройството работи и е в режим на засичане**.
**Работа с бутона:**
При нормална работа на измерване бутонът се ползва за блокировка на измерването. Например преминава кола или обект, който не трябва да бъде отчетен. Има две възможни опции:
- при натискане и задържане на бутона, зеленият светодиод започва да премигва бързо на кратки интервали (50 мс светване и 200 мс интервал). **Това означава, че измерването е блокирано и преминаване пред сензора няма да бъде отчетено**. Ако бутонът се пусне и не е натиснат измерването продължава нормално като се индикира от зеления светодиод (продължителност 75 милисекунди, през 1 сек).
- при двукратно натискане на бутона в рамките на 2 сек. устройството минава в режим на перманентна блокировка, зеленият светодиод започва да премигва бързо на кратки интервали (50 мс светване и 200 мс интервал), но не е нужно да се държи натиснат бутона. Ако отново се натисне двукратно в интервал 2 сек, се връжа към нормална работа. **Тази функция е с цел да се нагласи на място устройството, и да се блокира измерването без да е нужно да се държи постоянно бутона докато е нужно.**
*Таблица 2, индикация на зеления светодиод*
Мигане | Статус
:---: | :---:
75 мс, през 1 сек | режим на измерване
50 мс, през, 200 мс | блокирано измерване

## Настройка на чувствителността на устройството 



