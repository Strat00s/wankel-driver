#pragma once

class LedRozmezi {
public:
  int pin = 0;
  int zacatek = 0;
  int konec = 0;
  bool sviti = false;
  const char *jmeno;

  LedRozmezi(int pin, int zacatek, int konec, const char *jmeno) : pin(pin), zacatek(zacatek), konec(konec), jmeno(jmeno) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  bool blikKontrola(int i) {
    bool svitila = sviti;
    if (i < konec && i >= zacatek) {
      sviti = true;
      digitalWrite(pin, HIGH);
    }
    else {
      sviti = false;
      digitalWrite(pin, LOW);
    }

    // vypis zmenu
    if (svitila != sviti) {
      //Serial.print(i);
      //Serial.print(" ");
      //Serial.print(jmeno);
      if (sviti) {
        //Serial.println(" on");
      }
      else {
        //Serial.println(" off");
      }
    }
    return sviti;
  }
};
