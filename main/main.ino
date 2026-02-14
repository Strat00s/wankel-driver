#include "LedRozmezi.hpp"
#include <SparkFun_TB6612.h>


#define POCET_SEGMENTU_CYKLU 100
#define HALL_ZMENA_LIMIT 70

#define MOTOR_STBY 6
#define MOTOR_AIN1 7
#define MOTOR_AIN2 8
#define MOTOR_PWMA 9

#define HALL_PIN A3

#define LED_MODRA_PIN 2
#define LED_ZLUTA_PIN 3
#define LED_CERVENA_PIN 4
#define LED_BILA_PIN 5


#define MOTOR_ZUBY 11.0                                         // pocet zubu na kole motoru
#define OSA_ZUBY 40.0                                           // pocet zubu na kole osy
#define OSA_MOTOR_POMER (OSA_ZUBY / MOTOR_ZUBY)                 // pocet otacek motoru potreba k jedne otacce osy
#define CYKLUS_OSA_POMER 3.0                                    // pocet otacek osy potreba k udelani jednoho celeho cyklu
#define CYKLUS_MOTOR_POMER (OSA_MOTOR_POMER * CYKLUS_OSA_POMER) // pocet otacek motoru potreba k udelani jednoho celeho cyklu


//Motor motor = Motor(MOTOR_AIN1, MOTOR_AIN2, MOTOR_PWMA, 1, MOTOR_STBY);
LedRozmezi modraLedka = LedRozmezi(LED_MODRA_PIN, 10, 20, "modra");
LedRozmezi zlutaLedka = LedRozmezi(LED_ZLUTA_PIN, 30, 40, "zluta");
LedRozmezi cervenaLedka = LedRozmezi(LED_CERVENA_PIN, 50, 60, "cervena");
LedRozmezi bilaLedka = LedRozmezi(LED_BILA_PIN, 60, 80, "bila");


int rozdil(int hodnota1, int hodnota2) {
  return abs(hodnota1 - hodnota2);
}


void setup() {
  pinMode(HALL_PIN, INPUT);
  pinMode(MOTOR_STBY, OUTPUT);
  pinMode(MOTOR_AIN1, OUTPUT);
  pinMode(MOTOR_AIN2, OUTPUT);
  pinMode(MOTOR_STBY, OUTPUT);
  Serial.begin(115200);
  Serial.println("START");

  digitalWrite(MOTOR_STBY, HIGH);
  digitalWrite(MOTOR_AIN1, HIGH);
  digitalWrite(MOTOR_AIN2, LOW);
  analogWrite(MOTOR_PWMA, 255);
}


bool magnet_blizko = false;
int segment = 0;
unsigned long minula_otacka = 0;
unsigned long ms_na_segment = 65535;
unsigned long minuly_cas_dilu = 0;

void loop() {
  unsigned long ted = millis();

  // senzor vraci linearni napeti
  // kdyz nic neni u senzoru, vraci +- polovinu mezi 0 - 5V
  // atmega168 ma 10bit ADC -> polovina = 2^10 / 2 = 512
  // zajima nas hall_zmena/rozdil od poloviny, takze absolutni hodnota poloviny - hodnota senzoru
  int hall_zmena = rozdil(analogRead(HALL_PIN), 512);

  // magnet se oddaluje, ale musime vyhladit mozne rychle zmeny -> + 30 at je zmena dost velka
  // dalsi moznost je zprumerovat hodnotu treba 5x po sobe a vyhladit to tak
  if (hall_zmena  + 30 < HALL_ZMENA_LIMIT && magnet_blizko) {
    magnet_blizko = false;
  }
  // zmena je dost velka -> magnet je dost blizko -> nova otacka
  else if (hall_zmena>= HALL_ZMENA_LIMIT && !magnet_blizko) {
    magnet_blizko = true;

    unsigned long ms_na_otacku = ted - minula_otacka;
    minula_otacka = ted;
    ms_na_segment = (ms_na_otacku * CYKLUS_MOTOR_POMER) / POCET_SEGMENTU_CYKLU;

    Serial.print("Cas cyklu: ");
    Serial.println(ms_na_otacku * CYKLUS_MOTOR_POMER);
    Serial.print("Cas na dil: ");
    Serial.println(ms_na_segment);
  }

  // ubehl cas sehmentu -> posun se na dalsi
  if (ted - minuly_cas_dilu >= ms_na_segment) {
    minuly_cas_dilu = ted;
    segment++;
  }

  modraLedka.blikKontrola(segment);
  cervenaLedka.blikKontrola(segment);
  zlutaLedka.blikKontrola(segment);
  bilaLedka.blikKontrola(segment);

  // vyresetuj segment kdyz jsme na konci
  if (segment > POCET_SEGMENTU_CYKLU) {
    segment = 0;
  }
}
