/*
  LiquidCrystal Library - Hello World

  Demonstrates the use a 20x4 LCD display.  The LiquidCrystal
  library works with all LCD displays that are compatible with the
  Hitachi HD44780 driver. There are many of them out there, and you
  can usually tell them by the 16-pin interface.

  This sketch prints "Hello World!" to the LCD
  and shows the time.

  The circuit:
   LCD RS pin to digital pin 9
   LCD Enable pin to digital pin 8
   LCD D4 pin to digital pin 7
   LCD D5 pin to digital pin 6
   LCD D6 pin to digital pin 5
   LCD D7 pin to digital pin 4
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VDD pin to 5V
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)

  Library originally added 18 Apr 2008
  by David A. Mellis
  library modified 5 Jul 2009
  by Limor Fried (http://www.ladyada.net)
  example added 9 Jul 2009
  by Tom Igoe
  modified 22 Nov 2010
  by Tom Igoe
  modified 7 Nov 2016
  by Arturo Guadalupi
  modified 04/05/2020
  by Anouar AHMADI to test the Irrigation pump card (20x4 LCD display)
  https://github.com/ahmadi-anouar/Smart-Automated-Irrigation-System

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld

*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
const int LCD_colimns = 20, LCD_rows = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(LCD_colimns, LCD_rows);
  // Print a message to the LCD in the first line.
  lcd.print("Hello World!");
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  lcd.print("LCD display test");
}

void loop() {
  // set the cursor to column 0, line 2
  // (note: line 2 is the third row, since counting begins with 0):
  lcd.setCursor(0, 2); 
  // lcd.setCursor(0, 1); // if you are testing a 16x2 LCD to write in the second line
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);
}
