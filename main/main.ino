#include "LedRozmezi.hpp"
#include <SparkFun_TB6612.h>


#define POCET_SEGMENTU_CYKLU 300
#define HALL_ZMENA_LIMIT 70
#define OTACKY_NA_CYKLUS 3.0 // pocet otacek osy potreba k udelani jednoho celeho cyklu

#define MOTOR_STBY 6
#define MOTOR_AIN1 7
#define MOTOR_AIN2 8
#define MOTOR_PWMA 9

#define HALL_PIN A3

#define LED_MODRA_PIN 3
#define LED_ZLUTA_PIN 4
#define LED_CERVENA_PIN 5
#define LED_BILA_PIN 2


//Motor motor = Motor(MOTOR_AIN1, MOTOR_AIN2, MOTOR_PWMA, 1, MOTOR_STBY);
LedRozmezi modraLedka = LedRozmezi(LED_MODRA_PIN, 20, 90, "modra");
LedRozmezi zlutaLedka = LedRozmezi(LED_ZLUTA_PIN, 100, 145, "zluta");
LedRozmezi cervenaLedka = LedRozmezi(LED_CERVENA_PIN, 145, 190, "cervena");
LedRozmezi bilaLedka = LedRozmezi(LED_BILA_PIN, 200, 280, "bila");


unsigned long cas_minule_otacky = 0;     // jak dlouho trvala posledni otacka
unsigned long cas_minuleho_segmentu = 0; // cas kdy se preslo do minuleho segmentu
double us_na_segment = 0xFFFFFFFF;       // prumerny cas jak dlouho trva jeden segment

int otacka_osy = 0;         // pocitadlo otacek osy. 3 otacky = 1 cyklus motoru
int minula_otacka_osy = 0;  // minuly stav pocitadla otacek osy

bool magnet_blizko = false; // bool pro "vypinani" kdy se reaguje na pozici magnetu
int segment = 0;            // pocitadlo kde se nachazime ve "virtualnim" poli


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
  digitalWrite(MOTOR_AIN1, LOW);
  digitalWrite(MOTOR_AIN2, HIGH);
  analogWrite(MOTOR_PWMA, 255);
}

void loop() {
  unsigned long ted = micros(); // cas teto smycky

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

    // pocitadlo otacek osy vzhledem k cyklu
    otacka_osy++;
    if (otacka_osy == 3) {
      otacka_osy = 0;
    }

    unsigned long us_na_otacku = ted - cas_minule_otacky;                     // vypocet casu v us ktery jak dlouho trva jedna otacka
    cas_minule_otacky = ted;                                                  // ulozeni casu posledni otacky
    us_na_segment = (us_na_otacku * OTACKY_NA_CYKLUS) / POCET_SEGMENTU_CYKLU; // vypocet jak dlouho bude zhruba trvat jeden segment v us

    Serial.print("Cas cyklu: ");
    Serial.print(us_na_otacku * OTACKY_NA_CYKLUS / 1000);
    Serial.println("ms");
    Serial.print("Cas otacky: ");
    Serial.print(us_na_otacku / 1000);
    Serial.println("ms");
    Serial.print("Cas na segment: ");
    Serial.print(us_na_segment / 1000);
    Serial.println("ms");
  }

  // nedelej nic dokud se nevypocte prvni otacka
  if (cas_minule_otacky == 0) {
    return;
  }

  // ubehl cas segmentu -> posun se na dalsi
  if (ted - cas_minuleho_segmentu >= us_na_segment) {
    cas_minuleho_segmentu += us_na_segment;
    segment++;
  }

  // synchronizace segmentu kazdou otacku
  if (otacka_osy != minula_otacka_osy) {
    minula_otacka_osy = otacka_osy;
    //int wanted = POCET_SEGMENTU_CYKLU / OTACKY_NA_CYKLUS * otacka_osy;
    //if (wanted != segment) {
    //  Serial.print("sync ");
    //  Serial.print(wanted);
    //  Serial.print(" ");
    //  Serial.println(segment);
    //}
    segment = POCET_SEGMENTU_CYKLU / OTACKY_NA_CYKLUS * otacka_osy;
  }

  // vyresetovani pocitadla segmentu
  if (segment >= POCET_SEGMENTU_CYKLU) {
    segment = 0;
  }

  // vsem LED se preda nynejsi segment a podle toho se rozsviti nebo zhasnou
  modraLedka.blikKontrola(segment);
  cervenaLedka.blikKontrola(segment);
  zlutaLedka.blikKontrola(segment);
  bilaLedka.blikKontrola(segment);
}
