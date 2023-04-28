/*-----------------------------------------------------------------------------------------------------
  Sol Wiemeyer, Josefina Perez Roca e Isabella Mastrángelo
  5LA 2023

  TP 1 Ej1: Crear un programa con su máquina de estado en la cual muestre la hora sin
  conectarse a internet . Consta de 2 pantallas en la primera se muestra la hora y la temperatura actual .
  En la segunda pantalla se puede setear la hora y los minutos.

  ----------------------------------------------------------------------------------------------------- */

/**********LIBRERIAS**************/
#include "Arduino.h"
#include "ESP32Time.h"
#include "time.h"
#include "WiFi.h"
#include "U8g2lib.h"
#include "string"
#include "DHT_U.h"
#include "Adafruit_Sensor.h"


/*********CONSTRUCTORES Y VARIABLES GLOBALES**********/
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int maquina = 0;

#define BTN_HORA 35
#define BTN_MIN 34

#define pantalla1 0
#define pantalla2 1
#define limpiar1 2
#define limpiar2 3
#define antiRebote1 4
#define antiRebote2 5

struct tm timeinfo;
ESP32Time rtc;

int hora;
int minuto;

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  dht.begin();
  rtc.setTime(0, 0, 0, 28, 4, 2023);
  pinMode(BTN_HORA, INPUT_PULLUP);
  pinMode(BTN_MIN, INPUT_PULLUP);

  hora = rtc.getHour(true);
  minuto = rtc.getMinute();
}

void loop() {
  float t = dht.readTemperature();
  Serial.println(t);
  char temp[5];
  sprintf(temp, "%2.1f", t);
  Serial.println(temp);

  char horaString[2];
  sprintf(horaString, "%i", hora);

  char minString[2];
  sprintf(minString, "%i", minuto);

  Serial.print(hora);
  Serial.print(":");
  Serial.println(minuto);

  switch (maquina)
  {
    case pantalla1:
      Serial.println("pantalla 1");
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB14_tr);
      u8g2.drawStr(0, 12, "Temperatura: ");
      u8g2.drawStr(5, 27, temp);
      u8g2.drawStr(90, 27, "C");
      u8g2.drawStr(0, 47, "Hora:");
      u8g2.drawStr(0, 62, horaString);
      u8g2.drawStr(25, 62, ":");
      u8g2.drawStr(35, 62, minString);
      u8g2.sendBuffer();

      if (digitalRead(BTN_HORA) == LOW && digitalRead(BTN_MIN) == LOW) {
        maquina = limpiar1;
      }
      break;

    case limpiar1:
      Serial.println("limpiar 1");
      u8g2.clearBuffer();
      u8g2.sendBuffer();
      if (digitalRead(BTN_HORA) == HIGH && digitalRead(BTN_MIN) == HIGH) {
        maquina = pantalla2;
      }
      break;

    case pantalla2:
      Serial.println("pantalla 2");
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB14_tr);
      u8g2.drawStr(0, 15, "Hora:");
      u8g2.drawStr(0, 30, horaString);
      u8g2.drawStr(25, 30, ":");
      u8g2.drawStr(35, 30, minString);
      u8g2.sendBuffer();
      if (digitalRead(BTN_HORA) == LOW && digitalRead(BTN_MIN) == LOW) {
        maquina = limpiar2;
      }
      if (digitalRead(BTN_HORA) == LOW && digitalRead(BTN_MIN) == HIGH) {
        maquina = antiRebote1;
      }
      if (digitalRead(BTN_MIN) == LOW && digitalRead(BTN_HORA) == HIGH) {
        maquina = antiRebote2;
      }
      break;

    case limpiar2:
      Serial.println("limpiar 2");
      u8g2.clearBuffer();
      u8g2.sendBuffer();
      if (digitalRead(BTN_HORA) == HIGH && digitalRead(BTN_MIN) == HIGH) {
        maquina = pantalla1;
      }
      break;

    case antiRebote1:
      Serial.println("antirrebote 1");
      u8g2.clearBuffer();
      u8g2.sendBuffer();
      if (digitalRead(BTN_HORA) == HIGH) {
        hora++;
        if (hora > 24)
        {
          hora = 0;
        }
        maquina = pantalla2;
      }
      break;

    case antiRebote2:
      Serial.println("antirebote 2");
      u8g2.clearBuffer();
      u8g2.sendBuffer();
      if (digitalRead(BTN_MIN) == HIGH) {
        minuto++;
        if (minuto > 60)
        {
          minuto = 0;
          hora++;
        }
        maquina = pantalla2;
      }
      break;
  }

}
