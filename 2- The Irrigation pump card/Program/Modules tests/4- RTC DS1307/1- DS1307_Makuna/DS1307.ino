// CONNECTIONS:
// DS1307 SDA --> SDA
// DS1307 SCL --> SCL
// DS1307 VCC --> 5v
// DS1307 GND --> GND

#define countof(a) (sizeof(a) / sizeof(a[0]))

#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS1307.h>
#include <LiquidCrystal.h>

RtcDS1307<TwoWire> Rtc(Wire);

const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup ()
{
  lcd.begin(20, 4);
  Rtc.Begin();

  if (!Rtc.IsDateTimeValid())
  {
    if (Rtc.LastError() != 0)
    {
      // we have a communications error
      // see https://www.arduino.cc/en/Reference/WireEndTransmission for
      // what the number means
      lcd.setCursor(0, 2);  lcd.print("COMM.ERROR ");  lcd.print(Rtc.LastError());
    }
    else
    {
      lcd.setCursor(0, 2);
      lcd.print("RTC lost confidence!");
      //      Rtc.SetDateTime(compiled);
    }
  }

  if (!Rtc.GetIsRunning())
  {
    lcd.setCursor(0, 2);    lcd.print("RTC starting NOW   ");
    Rtc.SetIsRunning(true);
  }
  //   Rtc.SetDateTime(compiled);
  
  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  // not needed in our project (Smart_Automated_Irrigation_System)
  Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low);
}

void loop ()
{
  if (!Rtc.IsDateTimeValid())
  {
    if (Rtc.LastError() != 0)
    {
      // we have a communications error
      // see https://www.arduino.cc/en/Reference/WireEndTransmission for
      // what the number means
      lcd.setCursor(0, 2);
      lcd.print("RTC comm.error = ");
      lcd.print(Rtc.LastError());
    }
    else
    {
      // Common Causes:
      //    1) the battery on the device is low or even missing and the power line was disconnected
      //      Serial.println("RTC lost confidence in the DateTime!");
      lcd.setCursor(0, 2);      lcd.print("RTC lost confidence!");
    }
  }

  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  delay(1000);
}

void printDateTime(const RtcDateTime& dt)
{
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u:%02u:%02u %02u/%02u/%04u"),
             
             dt.Hour(),
             dt.Minute(),
             dt.Second(), 
             dt.Day(),
             dt.Month(),
             dt.Year());
  lcd.setCursor(0, 0);
  lcd.print(datestring);
}
