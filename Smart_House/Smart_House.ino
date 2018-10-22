#include "DHT.h"

/* Добавляем библиотеку для IR */
#include "IRremote.h"

/* Добавляем библиотеку для дисплея, на будущее)). ведь можернизация проекта должна включать и его ))*/
#include <iarduino_OLED_txt.h>


// *************************************** Начало настроек *****************************
/*
 * Алгоритм работы 
    1)  Короткое нажатие на любую кнопку включает\выключает соответствующий свет
    2) Длительное нажатие на любую кнопку выключает все группы света
    3) Длительное (3 сек) одновременное нажатие на 1 и 4 кнопки включает \ отключает автоматический режим работы клапана обогрева
    о чем свидетельствует плавное включение, выключение сигнальных светодиодов
    4) нажатие на 2 и 3 кнопки на пол секунды включает \выключает клапан вручную в том случае, если авто режим работы отключен
    
    5) Тумблер - работает на вкл и выкл., но при длительном удержании "мастер кнопок" (из 6 в проекте), 
    должны выключаться все световые приборы, в том числе управляемые тумблерами. Возобновление работы устройсв, включающих тумблер
    должно происходить если вернуть его в положение выкл и снова включить (пока как-то так=) ) */
    
    
// к какому пину подключен сигнальный контакт датчика DHT
#define DHTPIN 52  /*ПЕРЕназначил пин*/   
// максимальная температура на которой клапан будет отключаться в режиме авто
//важно ее указывать с точкой и двумя нулями после
#define MAXTEMPERATURE 22.00

// тип датчика температуры, раскомментировать нужную из трех строк
#define DHTTYPE DHT22   
//#define DHTTYPE DHT22   
//#define DHTTYPE DHT21   

// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// указываем, к каким пинам будут подключены кнопки
// не использовать пины А6, А7
//кнопка подключается одним концом к пину ардуино, другим к Gnd
/*ПЕРЕназначил пины*/
#define BUTTON1 22          // кнопка включения лампы №1 /* Освещение 1 */
#define BUTTON2 24          // кнопка включения лампы №2 /* Освещение 2 */
#define BUTTON3 26          // кнопка включения лампы №3 /* Освещение периметра */
#define BUTTON4 28          // кнопка включения светодиодной подсветки. /* Cветодиодная лента RGBW*/

/*Добавляем новые кнопки и тумблеры*/
#define BUTTON5 30          /* Кнопка: Ночник для чтения 1 */
#define BUTTON6 32          /* Кнопка: Ночник для чтения 2 */
#define BUTTON7 34          /* Тумблер: Зона над подиумом 1 */     
#define BUTTON8 36          /* Тумблер: Зона над подиумом 2 */
#define BUTTON9 38          /* Тумблер: Зона над подиумом 3 */
#define BUTTON10 40         /* Тумблер: Зона над подиумом 4 */
#define BUTTON11 42         /* Тумблер: Зона над подиумом 2 */
#define BUTTON12 44         /* Тумблер: Ультрафиолетовая лампа */
#define BUTTON13 46         /* Тумблер: Светомузыка */
#define BUTTON14 48         /* Тумблер: Cветодиодная лента RGBW (подиум) */
#define BUTTON15 50         /* Тумблер: Резерв */

// в зависимости от типа кнопки, если это обычная механическая кнопка, 
//подключенная к GND, то устанавливаем ACTIVE 0 
// если электронная кнопка, возможно ACTIVE 1
#define ACTIVE 0

//указываем к каким пинам будут подключены реле для ламп
/*ПЕРЕназначил пины*/ 
#define LAMP1 23           /*Реле. Освещение 1*/
#define LAMP2 25           /*Реле. Освещение 2*/
#define LAMP3 27           /*Реле. Освещение периметра*/
#define RGBWSTRIP 29       /*Реле. Cветодиодная лента RGBW (периметр)*/

/* Новые реле:*/
#define nightlight1 31     /* Реле. Ночник для чтения 1*/
#define nightlight2 33     /* Реле. Ночник для чтения 2*/
#define cornice 35         /* Реле. Освещение карниза*/
#define theatrelight1 37   /* Реле. Зона над подиумом 1*/
#define theatrelight2 39   /* Реле. Зона над подиумом 1*/
#define theatrelight3 41   /* Реле. Зона над подиумом 1*/
#define theatrelight4 43   /* Реле. Зона над подиумом 1*/
#define uvlight 45         /* Реле. Ультрафиолетовая лампа*/
#define lightmusic 47      /* Реле. Светомузыка*/
#define rgbwstrip2 49      /* Реле. Cветодиодная лента RGBW (подиум)*/
#define rezerv 51          /* Реле. Резерв*/

// тип управления реле
// если включается нулем, оставить как есть, если включается единицей, поменять 1 и 0 местами
// чтобы убедиться что эти параметры установлены правильно, при первом включении после перепрошивки
// все лампы должны быть выключены, а все подсветки включателей - светиться
#define RELAY_ON 0
#define RELAY_OFF 1 

// пин клапана воды
#define KLAPAN 53

//к каким пинам будут подключены светодиоды подсветки включаетелей
// светодиод подключается одной ногой (анод) к пину ардуино, второй ногой (катод) через резистор 150-1000 Ом  к Gnd

/*ПЕРЕназначил пины*/ 
#define LEDLAMP1 2
#define LEDLAMP2 3
#define LEDLAMP3 4
#define LEDRGBWSTRIP 5

/* Новые светодиод (состояния)*/
#define LEDKLAPAN 6 /*(будем потом плавно включать и выключать)*/

// для вывода в консоль отладочных сообщений расскомментировать
//#define DEBUG
// *************************************** конец настроек *****************************

/*** IR ***/

/*назначаем Pin 11 для IR*/
IRrecv irrecv (11);
/*Создаем переменную для IR */
decode_results results;

/* Переменные для кодов IR */
/* Gозже заменим значения на данные кнопок пульта */ 
unsigned long IR_lamp1 = 12345678;         /* IR: Освещение 1*/
unsigned long IR_lamp2 = 12345678;         /* IR: Освещение 2*/
unsigned long IR_lamp3 = 12345678;         /* IR: Освещение периметра*/
unsigned long IR_lamp4 = 12345678;         /* IR: Cветодиодная лента RGBW (периметр)*/
unsigned long IR_nightlight1 = 12345678;   /* IR: Ночник для чтения 1*/
unsigned long IR_nightlight2 = 12345678;   /* IR: Ночник для чтения 2*/
unsigned long IR_cornice = 12345678;       /* IR: Освещение карниза*/
unsigned long IR_theatrelight1 = 12345678; /* IR: Зона над подиумом 1*/
unsigned long IR_theatrelight2 = 12345678; /* IR: Зона над подиумом 2*/
unsigned long IR_theatrelight3 = 12345678; /* IR: Зона над подиумом 3*/
unsigned long IR_theatrelight4 = 12345678; /* IR: Зона над подиумом 4*/
unsigned long IR_uvlight = 12345678;       /* IR: Ультрафиолетовая лампа*/
unsigned long IR_lightmusic = 12345678;    /* IR: Светомузыка*/
unsigned long IR_rgbwstrip2 = 12345678;    /* IR: Cветодиодная лента RGBW (подиум)*/
unsigned long IR_rezerv = 12345678;        /* IR: Резерв */

DHT dht(DHTPIN, DHTTYPE);

bool statelamp1 = 0;                       /* Состояние: Освещение 1*/
bool statelamp2 = 0;                       /* Состояние: Освещение 2*/  
bool statelamp3 = 0;                       /* Состояние: Освещение периметра*/
bool statergbwstrip = 0;                   /* Состояние: Cветодиодная лента RGBW (периметр)*/
bool stateklapan = 0; // состояние клапан открыто \ закрыто
bool klapanmode = 0; // состояние режима работы клапана. 0 - ручной режим, 1 - авто режим от датчика температуры

/* Состояние новых приборов (1)*/
bool statenightlight1 = 0;                 /* Состояние: Ночник для чтения 1*/
bool statenightlight2 = 0;                 /* Состояние: Ночник для чтения 2*/

/* Состояние новых приборов (2)*/
bool statecornice = 0;                     /* Состояние: Освещение карниза*/
bool statetheatrelight1 = 0;               /* Состояние: Зона над подиумом 1*/
bool statetheatrelight2 = 0;               /* Состояние: Зона над подиумом 2*/
bool statetheatrelight3 = 0;               /* Состояние: Зона над подиумом 3*/
bool statetheatrelight4 = 0;               /* Состояние: Зона над подиумом 4*/
bool stateuvlight = 0;                     /* Состояние: Ультрафиолетовая лампа*/
bool statelightmusic = 0;                  /* Состояние: Светомузыка*/
bool statergbwstrip2 = 0;                  /* Состояние: Cветодиодная лента RGBW (подиум)*/
bool staterezerv = 0;                      /* Состояние: Резерв*/


byte autoBut = 0; // автомат обработки всех 4  кнопок 
byte autoButprev  = 0; // для отладки
unsigned long ms =0, butMs = 0;
// счетчик верных нажатий ( отпусканий) той или иной кнопки
byte butcount = 0;

byte autoKlapan = 0; // автомат обработки клапона. управляется из автомата кнопок
unsigned long klapanMs = 0;


void setup() {
  Serial.begin(115200);
  dht.begin();
    
/* запускаем IR*/
  irrecv.enableIRIn();
    
pinMode(BUTTON1, INPUT_PULLUP);            /* Копка/Вход с подтяжкой: Освещение 1*/
pinMode(BUTTON2, INPUT_PULLUP);            /* Копка/Вход с подтяжкой: Освещение 2*/
pinMode(BUTTON3, INPUT_PULLUP);            /* Копка/Вход с подтяжкой: Освещение периметра*/
pinMode(BUTTON4, INPUT_PULLUP);            /* Копка/Вход с подтяжкой: Cветодиодная лента RGBW (периметр)*/

/* Новые Кнопки: */
pinMode (BUTTON5, INPUT_PULLUP);           /* Копка: Вход - Ночник для чтения 1. */
pinMode (BUTTON6, INPUT_PULLUP);           /* Копка: Вход - Ночник для чтения 2* /

/* Тумблеры ВКЛ/ВЫКЛ. */
pinMode (BUTTON7, INPUT);                  /* Тумблер ВКЛ/ВЫКЛ: Вход - Зона над подиумом 1 */
pinMode (BUTTON8, INPUT);                  /* Тумблер ВКЛ/ВЫКЛ: Вход - Зона над подиумом 2 */
pinMode (BUTTON9, INPUT);                  /* Тумблер ВКЛ/ВЫКЛ: Вход - Зона над подиумом 3 */
pinMode (BUTTON10, INPUT);                 /* Тумблер ВКЛ/ВЫКЛ: Вход - Зона над подиумом 4 */
pinMode (BUTTON11, INPUT);                 /* Тумблер ВКЛ/ВЫКЛ: Вход - Зона над подиумом */
pinMode (BUTTON12, INPUT);                 /* Тумблер ВКЛ/ВЫКЛ: Вход - Ультрафиолетовая лампа */
pinMode (BUTTON13, INPUT);                 /* Тумблер ВКЛ/ВЫКЛ: Вход - Светомузыка */
pinMode (BUTTON14, INPUT);                 /* Тумблер ВКЛ/ВЫКЛ: Вход - Cветодиодная лента RGBW (подиум) */
pinMode (BUTTON15, INPUT);                 /* Тумблер ВКЛ/ВЫКЛ: Вход - Резерв */
    
 
/*Реле*/    
pinMode(LAMP1, OUTPUT);
pinMode(LAMP2, OUTPUT);
pinMode(LAMP3, OUTPUT);
pinMode(RGBWSTRIP, OUTPUT);

/*  Новые Реле:*/
pinMode(nightlight1, OUTPUT);    // Реле: Выход - Ночник для чтения 1
pinMode(nightlight2, OUTPUT);    // Реле: Выход - Ночник для чтения 2
pinMode(cornice, OUTPUT);        // Реле: Выход - Освещение карниза
pinMode(theatrelight1, OUTPUT);  // Реле: Выход - Зона над подиумом 1
pinMode(theatrelight2, OUTPUT);  // Реле: Выход - Зона над подиумом 2
pinMode(theatrelight3, OUTPUT);  // Реле: Выход - Зона над подиумом 3
pinMode(theatrelight4, OUTPUT);  // Реле: Выход - Зона над подиумом 4
pinMode(uvlight, OUTPUT);        // Реле: Выход - Ультрафиолетовая лампа
pinMode(lightmusic, OUTPUT);     // Реле: Выход - Светомузыка
pinMode(rgbwstrip2, OUTPUT);     // Реле: Выход - Cветодиодная лента RGBW (подиум)
pinMode(rezerv, OUTPUT);         // Реле: Выход - Резерв    
        
/* Светодиоды */
pinMode(LEDLAMP1, OUTPUT);
pinMode(LEDLAMP2, OUTPUT);
pinMode(LEDLAMP3, OUTPUT);
pinMode(LEDRGBWSTRIP, OUTPUT);

/* Новый светодиод */
pinMode(LEDKLAPAN, OUTPUT);
    
/*Клапан*/    
pinMode(KLAPAN, OUTPUT);

digitalWrite(KLAPAN , RELAY_OFF);
makelamps(); // включим лампы и подсветку как надо
}//setup

void loop() {

ms = millis();
autoButfunc(); // опрос кнопок и работа света
autoKlapanfunc(); // работа клапана
IRfunc(); /* Добавил функцию для IR*/

}//loop
