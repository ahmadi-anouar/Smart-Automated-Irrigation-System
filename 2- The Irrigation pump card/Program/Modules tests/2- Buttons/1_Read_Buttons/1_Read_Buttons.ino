#include <LiquidCrystal.h>


/*****************************************
   Definitions
 *****************************************/
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
const int LCD_colimns = 20, LCD_rows = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int adc_key_in = 0;
#define BUTTONS A3
#define LCD_SW 2

/*******************************************
   Setup
 *******************************************/
void setup()
{
  // initialize the switch button pin as an input:
  pinMode(LCD_SW, INPUT);
  // initialize the LCD display
  lcd.begin(20, 4); // start
  lcd.setCursor(0, 0);  lcd.print("Testing Mode");
  lcd.setCursor(0, 1);  lcd.print("Buttons test");
  lcd.setCursor(0, 2);  lcd.print("Press any button!");
  lcd.setCursor(0, 3);  lcd.print("ADC=0");
  lcd.setCursor(10, 3);  lcd.print("LCD_SW=");
}

/*****************************************************
   Main loop
 ******************************************************/

void loop()
{
  adc_key_in = analogRead(A3); // read value
  lcd.setCursor(4, 3);  lcd.print("    ");
  lcd.setCursor(4, 3);  lcd.print(adc_key_in);
  lcd.setCursor(17, 3);
  if (!digitalRead(LCD_SW)) {
    lcd.print("ON ");
  } else {
    lcd.print("OFF");
  }
  delay(1000);
}
