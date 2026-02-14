#include "LedRozmezi.hpp"
#include <SparkFun_TB6612.h>


#define DELKA_CYKLU 100
#define HALL_ZMENA_LIMIT 70

#define MOTOR_STBY 6
#define MOTOR_AIN1 7
#define MOTOR_AIN2 8
#define MOTOR_PWMA 9

#define HALL_PIN A3

#define LED_MODRA_PIN 3
#define LED_ZLUTA_PIN 4
#define LED_CERVENA_PIN 5
#define LED_BILA_PIN 6


#define MOTOR_ZUBY 11.0                                         // pocet zubu na kole motoru
#define OSA_ZUBY 40.0                                           // pocet zubu na kole osy
#define OSA_MOTOR_POMER (OSA_ZUBY / MOTOR_ZUBY)                 // pocet otacek motoru potreba k jedne otacce osy
#define CYKLUS_OSA_POMER 3.0                                    // pocet otacek osy potreba k udelani jednoho celeho cyklu
#define CYKLUS_MOTOR_POMER (OSA_MOTOR_POMER * CYKLUS_OSA_POMER) // pocet otacek motoru potreba k udelani jednoho celeho cyklu


Motor motor = Motor(MOTOR_AIN1, MOTOR_AIN2, MOTOR_PWMA, 1, MOTOR_STBY);
LedRozmezi modraLedka = LedRozmezi(LED_MODRA_PIN, 10, 20, "modra");
LedRozmezi zlutaLedka = LedRozmezi(LED_ZLUTA_PIN, 30, 40, "zluta");
LedRozmezi cervenaLedka = LedRozmezi(LED_CERVENA_PIN, 50, 60, "cervena");
LedRozmezi bilaLedka = LedRozmezi(LED_BILA_PIN, 60, 80, "bila");


int rozdil(int hodnota1, int hodnota2) {
  return abs(hodnota1 - hodnota2);
}


void setup() {
  pinMode(HALL_PIN, INPUT);
  Serial.begin(115200);
  motor.drive(100);
}


bool magnet_blizko = false;
int loop_cnt = 0;
int old_zmena = 0;
void loop() {
  //modraLedka.blikKontrola(i);
  //cervenaLedka.blikKontrola(i);
  //zlutaLedka.blikKontrola(i);
  //bilaLedka.blikKontrola(i);

  // senzor vraci linearni napeti
  // kdyz nic neni u senzoru, vraci +- polovinu mezi 0 - 5V
  // atmega168 ma 10bit ADC -> polovina = 2^10 / 2 = 512
  // zajima nas hall_zmena/rozdil od poloviny, takze absolutni hodnota poloviny - hodnota senzoru
  int hall_zmena = rozdil(analogRead(HALL_PIN), 512);

  // magnet je blizko, kdyz je hall_zmena dostatecne velka
  if (hall_zmena  + 30 < HALL_ZMENA_LIMIT && magnet_blizko) {
    magnet_blizko = false;
    Serial.print(loop_cnt);
    Serial.println(": daleko");
  }
  else if (hall_zmena >= HALL_ZMENA_LIMIT && !magnet_blizko) {
    magnet_blizko = true;
    loop_cnt++;
    Serial.print(loop_cnt);
    Serial.println(": blizko");
  }

  if (abs(old_zmena - hall_zmena) > 10) {
    old_zmena = hall_zmena;
    Serial.print("A: ");
    Serial.println(hall_zmena);
  }

  //delay(10);

  if (i == DELKA_CYKLU) {
    i = 0;
  }
  return;
}
