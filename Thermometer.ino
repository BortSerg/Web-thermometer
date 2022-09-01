#include <ArduinoOTA.h>
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <TM1637.h>
#include <EEPROM.h>
#include <SPIFFS.h>

BME280I2C::Settings settings(
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::Mode_Forced,
    BME280::StandbyTime_1000ms,
    BME280::Filter_Off,
    BME280::SpiEnable_False,
    0x76 // I2C address. I2C specific.
);
BME280I2C bme(settings);

// I2C pins                                 //Wemos ESP8266
#define SDA_pin 4 // D2 pin
#define SCL_pin 5 // D1 pin

// инициализация oneWire и DS18B20
#define ONE_WIRE_BUS D4          // вывод к которому подключен DS18B20 он же GPIO1
#define TEMPERATURE_PRECISION 12 // точность бит. Если глючит или врет, уменьшить с 12 до 9

// определение пинов для вывода на индикаор ТМ1637
#define CLK D6
#define DIO D5

//Создаём объект класса TM1637, в качестве параметров передаём номера пинов подключения
TM1637 tm1637(CLK, DIO);

int NumberOfDevices; //сколько датчиков DS18B20 в системе.

// Переменные для таймингов
unsigned long previousMillis_TM1637 = 0;

ESP8266HTTPUpdateServer httpUpdater;
ESP8266WebServer server(80);


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);



struct ESP_Config
{                      //структура для хранения настроек метеостанции в EEPROM памяти. Переменные структуры записаны большими буквами, для их отличия от других глобальных переменных
  char MUSOR[16] = {}; //В начале сектора EEPROM содержатся какие-то данные ESP, запись туда важных данных нежелательна
  // Наименование в режиме клиента
  char WIFI_SSID[32] = {}; //имя сети
  char WIFI_PASSWORD[32] = {};
  // Параметры подключения в режиме точки доступа
  char AP_SSID[32] = {}; //имя точки доступа
  char AP_PASSWORD[32] = {};
  // Настройки часов
  int GMT = 0;               //часовой пояс
  bool SYNC_CLOCK = 0;       //разрешение на синхронизацию
  uint8_t INTERVAL_SYNC = 0; //интервал синхронизации часов
  // Настройки отправки данных на narodmon.ru
  bool SEND_NM = 0;        //разрешение на отправку данных Narodmon
  uint8_t INTERVAL_NM = 0; //интервал отправки данных
  // Настройки отправки сообщений в Telegram
  bool SEND_TL = 0;        //разрешение на отправку данных Telegram
  uint8_t INTERVAL_TL = 0; //интервал отправки данных
  char CHAT_ID[16] = {};   //Уникальный ID  чата
  char TOKEN[64] = {};     // API Token
  byte LIGHT = 0;          //Яркость 7-сегментного индикатора
  byte CONTROLL_VALUE = 0; //Контрольное значение = 88. Используется для определения загрузки дефолтных настроек при первом запуске
} Config;

void setup()
{

  Serial.begin(115200);
  Serial.println("");
  Serial.println("Soft ver. 14.07.2022");
  Serial.println("");
  EEPROM.begin(sizeof(Config)); //инициализация EEPROM памяти в размере = размеру структуры Config
  Serial.println("Инициализированно " + String(sizeof(Config)) + " byte EEPROM памяти");

  OTA_Update(); //подключение обновления по воздуху

  Read_Config(); // xтение конфига из EEPROM памяти конфигураций

  if (Config.CONTROLL_VALUE != 88) // проверка считаных с памяти данных. Если контрольное значение не верно, то записать дефолтные настройки
  {
    String ssid = "WiFiName";
    String pass = "";
    String ap_ssid = "ESP8266";
    String ap_pass = "";
    strcpy(Config.WIFI_SSID, ssid.c_str());
    strcpy(Config.WIFI_PASSWORD, pass.c_str());
    strcpy(Config.AP_SSID, ap_ssid.c_str());
    strcpy(Config.AP_PASSWORD, ap_pass.c_str());
    Config.SEND_NM = 5;
    Config.GMT = 0;
    Config.SYNC_CLOCK = 1;
    Config.INTERVAL_NM = 5;
    Config.INTERVAL_SYNC = 12;
    Config.LIGHT = 2;
    Config.CONTROLL_VALUE = 88;
    save_Config();
  }

  TM1637_init(); //Инициализация модуля TM1637

  WiFi_init(); //подключение к сети и синхронизация часов/поднятие точки доступа

  Dalas18B20(); //поиск и инициализация датчиков DS18B20

  Init_html(); //инициализация HTML страниц сервера

  Serial.println("Temperature web server started!\n");


  IP_address_run(); //вывод IP адреса


}

void loop()
{
  server.handleClient();
  ArduinoOTA.handle();
  delay(100);

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis_TM1637 > interval_TM1637) //вывод температуры в помещении
  {
    tm1637.point(false);
    previousMillis_TM1637 = currentMillis;
    getTM1637TemperatureRoom();
    count++;
  }


  yield();
}
