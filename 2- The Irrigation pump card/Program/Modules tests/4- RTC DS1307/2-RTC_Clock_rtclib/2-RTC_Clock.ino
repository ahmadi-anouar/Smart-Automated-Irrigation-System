/*
  :Project:Clock_Set_Date_Time
  :Author: Tiziano Bianchettin
  :Date: 13/01/2017
  :Revision: 1
  :License: Public Domain
  thanks to:
  http://arduinoenonsolo.blogspot.it/2012/12/orologio-con-arduino-e-il-ds1307.html
  http://www.mauroalfieri.it/
  http://www.danielealberti.it/
  http://www.maffucci.it/
  My electronics laboratory professor "Perito Carli"
*/
//************libraries**************//
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>

//************************************//
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

DS1307 RTC;

int adc_key_in = 0;
int btn_pressed = 5;
#define BUTTONS A3
#define LCD_SW 2
#define btn_DOWN 0
#define btn_UP 1
#define btn_OK 2
#define btn_LEFT 3
#define btn_RIGHT 4
#define btn_NONE 5

#define countof(a) (sizeof(a) / sizeof(a[0]))

//************Variables**************//
int hourupg;
int minupg;
int yearupg;
int monthupg;
int dayupg;
int menu = 0;

char display_2[2];


void setup()
{

  lcd.begin(20, 4);
  lcd.clear();

  Serial.begin(9600);
  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // Set the date and time at compile time
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  // RTC.adjust(DateTime(__DATE__, __TIME__)); //removing "//" to adjust the time
  // The default display shows the date and time
  int menu = 0;
}

void loop()
{
  btn_pressed = read_LCD_buttons(); //read the buttons

  // check if you press the SET button and increase the menu index
  if (btn_pressed == btn_OK)
  {
    menu = menu + 1;
  }
  // in which subroutine should we go?
  if (menu == 0)
  {
    //    DisplayDateTime(); // void DisplayDateTime
    DateTime now = RTC.now();
    hourupg = now.hour();
    minupg = now.minute();
    dayupg = now.day();
    monthupg = now.month();
    yearupg = now.year();
    printDateTime(now);
  }
  if (menu == 1)
  {
    DisplaySetHour();
  }
  if (menu == 2)
  {
    DisplaySetMinute();
  }
  if (menu == 3)
  {
    DisplaySetDay();
  }
  if (menu == 4)
  {
    DisplaySetMonth();
  }
  if (menu == 5)
  {
    DisplaySetYear();
  }
  if (menu == 6)
  {
    StoreAgg();
    delay(500);
    menu = 0;
  }
  delay(100);
}

void DisplaySetHour()
{
  // time setting
  DateTime now = RTC.now();
  btn_pressed = read_LCD_buttons(); //read the buttons
  if (read_LCD_buttons() == btn_UP)
  {
    if (hourupg == 23)
    {
      hourupg = 0;
    }
    else
    {
      hourupg = hourupg + 1;
    }
  }
  if (read_LCD_buttons() == btn_DOWN)
  {
    if (hourupg == 0)
    {
      hourupg = 23;
    }
    else
    {
      hourupg = hourupg - 1;
    }
  }
  lcd.setCursor(0, 1);  lcd.print("Set houre           ");
  lcd.setCursor(0, 0);  lcd.print("  ");
  lcd.setCursor(0, 0);  lcd.print(hourupg, DEC);

  delay(200);
}

void DisplaySetMinute()
{
  // Setting the minutes
  if (read_LCD_buttons() == btn_UP)
  {
    if (minupg == 59)
    {
      minupg = 0;
    }
    else
    {
      minupg = minupg + 1;
    }
  }
  if (read_LCD_buttons() == btn_DOWN)
  {
    if (minupg == 0)
    {
      minupg = 59;
    }
    else
    {
      minupg = minupg - 1;
    }
  }
  lcd.setCursor(0, 1);  lcd.print("Set Minutes         ");
  lcd.setCursor(3, 0);  lcd.print("  ");
  lcd.setCursor(3, 0);  lcd.print(minupg, DEC);

  delay(200);
}

void DisplaySetYear()
{
  // setting the year
  if (read_LCD_buttons() == btn_UP)
  {
    yearupg = yearupg + 1;
  }
  if (read_LCD_buttons() == btn_DOWN)
  {
    yearupg = yearupg - 1;
  }
  lcd.setCursor(0, 1);    lcd.print("Set Year            ");
  lcd.setCursor(15, 0);   lcd.print(yearupg, DEC);
  delay(200);
}

void DisplaySetMonth()
{
  // Setting the month
  if (read_LCD_buttons() == btn_UP)
  {
    if (monthupg == 12)
    {
      monthupg = 1;
    }
    else
    {
      monthupg = monthupg + 1;
    }
  }
  if (read_LCD_buttons() == btn_DOWN)
  {
    if (monthupg == 1)
    {
      monthupg = 12;
    }
    else
    {
      monthupg = monthupg - 1;
    }
  }
  lcd.setCursor( 0, 1);  lcd.print("Set Month           ");
  lcd.setCursor(12, 0);  lcd.print("  ");
  lcd.setCursor(12, 0);  lcd.print(monthupg);
  delay(200);

}

void DisplaySetDay()
{
  // Setting the day
  if (read_LCD_buttons() == btn_UP)
  {
    if (dayupg == 31)
    {
      dayupg = 1;
    }
    else
    {
      dayupg = dayupg + 1;
    }
  }
  if (read_LCD_buttons() == btn_DOWN)
  {
    if (dayupg == 1)
    {
      dayupg = 31;
    }
    else
    {
      dayupg = dayupg - 1;
    }
  }
  lcd.setCursor(0, 1);  lcd.print("Set Day             ");
  lcd.setCursor(9, 0);  lcd.print("  ");
  lcd.setCursor(9, 0);  lcd.print(dayupg);
  delay(200);
}

void StoreAgg()
{
  // Variable saving
  lcd.setCursor(0, 1);  lcd.print("SAVING IN PROGRESS..");
  RTC.adjust(DateTime(yearupg, monthupg, dayupg, hourupg, minupg, 0));
  delay(1000);
  lcd.setCursor(0, 1);  lcd.print("                    ");
}

int read_LCD_buttons()
{
  adc_key_in = analogRead(A3); // read value

  if (adc_key_in > 750)  return btn_NONE;
  if (adc_key_in > 600)  return btn_RIGHT;
  if (adc_key_in > 400)  return btn_LEFT;
  if (adc_key_in > 255)  return btn_OK;
  if (adc_key_in > 65)   return btn_UP;
  return btn_DOWN;       // If all else fails, it's btn_DOWN ([0,65])
}

void printDateTime(const DateTime& dt)
{
  char datestring[20];
  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u:%02u:%02u %02u/%02u/%04u"),
             dt.hour(),
             dt.minute(),
             dt.second(),
             dt.day(),
             dt.month(),
             dt.year());
  lcd.setCursor(0, 0);
  lcd.print(datestring);
}
