#include <EEPROM.h>

byte value;
char s[20];
const int EE_Mode = 0, EE_Menu = 1, EE_POMPERemp = 2, EE_POMPEIrrig = 3, EE_Vanne = 4, EE_Slider = 5, EE_NIVEAU_inst = 6, EE_NIVEAU_low = 10, EE_NIVEAU_hi = 14;
float f;

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);  //Initialize EEPROM

  f=27.0;
  EEPROM.put(EE_NIVEAU_inst, f);
  f=17;
  EEPROM.put(EE_NIVEAU_low, f);
  f=77;
  EEPROM.put(EE_NIVEAU_hi, f);
  EEPROM.write(EE_Mode,       0);
  EEPROM.write(EE_Menu,       1);
  EEPROM.write(EE_POMPERemp,  0);
  EEPROM.write(EE_POMPEIrrig, 0);
  EEPROM.write(EE_Vanne,      0);
  EEPROM.write(EE_Slider,     0);

  EEPROM.commit();    //Store data to EEPROM

  
  Serial.println("Stored...");
  Serial.println("0-Mode 1-Menu 2-POMPERemp 3-POMPEIrrig 4-Vanne 5-Slider");
  for (int i = 0; i < 6; i++) {
    value = EEPROM.read(i);
    Serial.print(i);
    Serial.print("\t");
    Serial.println(value, DEC);
  }
  EEPROM.get(EE_NIVEAU_inst, f);
  Serial.print(EE_NIVEAU_inst);
  Serial.print("\t");
  snprintf(s, 6, "%f", f);
  Serial.print(s);
  Serial.println("\t NIVEAU_inst");

  EEPROM.get(EE_NIVEAU_low, f);
  Serial.print(EE_NIVEAU_low);
  Serial.print("\t");
  snprintf(s, 6, "%f", f);
  Serial.print(s);
  Serial.println("\t NIVEAU_low");

  EEPROM.get(EE_NIVEAU_hi, f);
  Serial.print(EE_NIVEAU_hi);
  Serial.print("\t");
  snprintf(s, 6, "%f", f);
  Serial.print(s);
  Serial.println("\t NIVEAU_hi");


}

void loop() {

}
