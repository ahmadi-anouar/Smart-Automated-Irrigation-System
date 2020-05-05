#include <LiquidCrystal.h>


/*****************************************
   Definitions
 *****************************************/
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
const int LCD_colimns = 20, LCD_rows = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

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

/* ---------Button Information--------------

  DOWN :    2 accepted: [  0, 65]
  UP   :  133 accepted: [ 66,225]
  OK   :  315 accepted: [226,400]
  LEFT :  483 accepted: [401,600]
  Right:  718 accepted: [601,750]
  NONE : 1019 accepted: [751,1023]

  -------------------------------------------*/

/*******************************************
  Setup
*******************************************/
void setup()
{
  // initialize the switch button pin as an input:
  pinMode(LCD_SW, INPUT);
  // initialize the LCD display
  lcd.begin(20, 4); // start
  lcd.setCursor(0, 0);  lcd.print("  * Testing Mode *  ");
  lcd.setCursor(0, 1);  lcd.print("Buttons test  SW=   ");
  lcd.setCursor(0, 2);  lcd.print("Press any button!   ");
  lcd.setCursor(0, 3);  lcd.print("ADC=                ");
}

/*****************************************************
   Main loop
 ******************************************************/

void loop()
{
  btn_pressed = read_LCD_buttons(); //read the buttons

  lcd.setCursor(4, 3);  lcd.print("    ");
  lcd.setCursor(4, 3);  lcd.print(adc_key_in);
  button_switch_case(btn_pressed);

  //Reading SW button
  lcd.setCursor(17, 1);
  if (!digitalRead(LCD_SW)) {
    lcd.print("ON ");
  } else {
    lcd.print("OFF");
  }
  delay(1000);
}

/**********************************************
   Reading the buttons
 ***********************************************/

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

/*********************************************************************************
   Switch case for buttons
 **********************************************************************************/

void button_switch_case(int btn_pressed)
{
  switch (btn_pressed)    //Depending on button pressed, perform action
  {

    /*******************************************************************************
       Right Button
     *******************************************************************************/
    case btn_RIGHT:
      {
        lcd.setCursor(9, 3);  lcd.print(" __RIGHT__ ");
        break;
      }
    /*******************************************************************************
       Left Button
     *******************************************************************************/
    case btn_LEFT:
      {
        lcd.setCursor(9, 3);  lcd.print(" __LEFT __ ");
        break;
      }
    /*******************************************************************************
       UP Button
     *******************************************************************************/
    case btn_UP:
      {
        lcd.setCursor(9, 3);  lcd.print(" __ UP  __ ");
        break;
      }

    /*******************************************************************************
       Down Button
     *******************************************************************************/
    case btn_DOWN:
      {
        lcd.setCursor(9, 3);  lcd.print(" __DOWN __ ");
        break;
      }

    /*******************************************************************************
       Select Button
     *******************************************************************************/
    case btn_OK:
      {
        lcd.setCursor(9, 3);  lcd.print(" __ OK  __ ");
        break;
      }
    /*******************************************************************************
       No Button Pressed
     *******************************************************************************/
    case btn_NONE:
      {
        lcd.setCursor(9, 3);  lcd.print(" __NONE __ ");
        break;
      }
  }
}
