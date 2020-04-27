/********************************************
                 Amra project
 ********************************************
    Last update code: 16/12/2019
    written by Anouar AHMADI
    collaborated wuth Ibtihel AHMADI
    Arduino nano (old bootloader) ATmega328P
    ________________________________________
    RF Communication : HC-12
    Ultrasound sensor: HC-SR04
    ----------------------------------------
    Send "N" to mesure distance in cm
 ********************************************/

#include <SoftwareSerial.h>

SoftwareSerial HC12(3, 2);                      /* HC-12 TX Pin, HC-12 RX Pin*/

const byte TRIGGER_PIN = 4;                     /* (A0) The ultrasound signal emission Pin HC-SR04*/
const byte ECHO_PIN = 5;                        /* (A1) The ultrasound signal receiving Pin*/
const float SOUND_SPEED = 343.0 / 1000;         /* Sound's speed 343 m/s*/

int input;

char val[8];

void setup()
{
  pinMode(TRIGGER_PIN, OUTPUT);                 /* Define the ultrasound output pin*/
  pinMode(ECHO_PIN, INPUT);                     /* Define the ultrasound enter pin*/
  HC12.begin(9600);                             /* Serial port to HC-12*/
  HC12.write("Send N for level in cm");

}

float Distance_cm()                             /* Measure the distance ahead*/
{
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);                         /* For low voltage 2 us ultrasonic launch*/
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);                        /* Let ultrasonic launch 10 us high voltage, there is at least 10 us*/
  digitalWrite(TRIGGER_PIN, LOW);               /* Maintaining low voltage ultrasonic launch*/
  long Time_spended = pulseIn(ECHO_PIN, HIGH);  /* Read the time difference*/
  return (Time_spended / 20.0) * SOUND_SPEED;
}

void loop()
{
  if (HC12.available()) {                       /* If HC-12 has data*/
    input = HC12.read();                        /* save it in input*/
  }

  if (input == int('N')) {
    HC12.write(dtostrf(Distance_cm(), 3, 2, val));
    HC12.write(10);
    input = 0;
  }
}
