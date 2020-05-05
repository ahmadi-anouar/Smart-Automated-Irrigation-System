#include <LiquidCrystal.h>


/*****************************************
   Definitions
 *****************************************/
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
const int LCD_colimns = 20, LCD_rows = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int     adc_key_in  = 0;
int     btn_pressed = 5;
#define BUTTONS   A3
#define LCD_SW    2
#define btn_DOWN  0
#define btn_UP    1
#define btn_OK    2
#define btn_LEFT  3
#define btn_RIGHT 4
#define btn_NONE  5
#define ALARM     A0
#define P_IRRIG   A1
#define E_VANNE   A2

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
  pinMode(LCD_SW,   INPUT);  // initialize the switch button pin as an input:
  pinMode(ALARM,    OUTPUT); digitalWrite(ALARM, HIGH);
  pinMode(P_IRRIG,  OUTPUT); digitalWrite(P_IRRIG, HIGH);
  pinMode(E_VANNE,  OUTPUT); digitalWrite(E_VANNE, HIGH);
  // initialize the LCD display
  lcd.begin(20, 4); // start
  lcd.setCursor(0, 0);  lcd.print("  * Testing Mode *  ");
  lcd.setCursor(0, 1);  lcd.print("Press:( )LEFT -ALARM");
  lcd.setCursor(0, 2);  lcd.print("      ( )DOWN -PUMP ");
  lcd.setCursor(0, 3);  lcd.print("      ( )RIGHT-VALVE");
}

/*****************************************************
   Main loop
 ******************************************************/

void loop()
{
  btn_pressed = read_LCD_buttons(); //read the buttons
  button_switch_case(btn_pressed);

  lcd.setCursor(0, 3);  // print the number of seconds since reset:
  lcd.print(millis() / 1000);

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
        lcd.setCursor(7, 1);  lcd.print(" "); digitalWrite(ALARM, HIGH);
        lcd.setCursor(7, 2);  lcd.print(" "); digitalWrite(P_IRRIG, HIGH);
        lcd.setCursor(7, 3);  lcd.print("X"); digitalWrite(E_VANNE, LOW);
        break;
      }
    /*******************************************************************************
       Left Button
     *******************************************************************************/
    case btn_LEFT:
      {
        lcd.setCursor(7, 1);  lcd.print("X"); digitalWrite(ALARM, LOW);
        lcd.setCursor(7, 2);  lcd.print(" "); digitalWrite(P_IRRIG, HIGH);
        lcd.setCursor(7, 3);  lcd.print(" "); digitalWrite(E_VANNE, HIGH);
        break;
      }
    /*******************************************************************************
       UP Button
     *******************************************************************************/
    case btn_UP:
      {
        lcd.setCursor(7, 1);  lcd.print(" "); digitalWrite(ALARM, HIGH);
        lcd.setCursor(7, 2);  lcd.print(" "); digitalWrite(P_IRRIG, HIGH);
        lcd.setCursor(7, 3);  lcd.print(" "); digitalWrite(E_VANNE, HIGH);
        break;
      }

    /*******************************************************************************
       Down Button
     *******************************************************************************/
    case btn_DOWN:
      {
        lcd.setCursor(7, 1);  lcd.print(" "); digitalWrite(ALARM, HIGH);
        lcd.setCursor(7, 2);  lcd.print("X"); digitalWrite(P_IRRIG, LOW);
        lcd.setCursor(7, 3);  lcd.print(" "); digitalWrite(E_VANNE, HIGH);
        break;
      }

    /*******************************************************************************
       Select Button
     *******************************************************************************/
    case btn_OK:
      {
        lcd.setCursor(7, 1);  lcd.print("X"); digitalWrite(ALARM, LOW);
        lcd.setCursor(7, 2);  lcd.print("X"); digitalWrite(P_IRRIG, LOW);
        lcd.setCursor(7, 3);  lcd.print("X"); digitalWrite(E_VANNE, LOW);
        break;
      }
    /*******************************************************************************
       No Button Pressed
     *******************************************************************************/
    case btn_NONE:
      {
        break;
      }
  }
}
