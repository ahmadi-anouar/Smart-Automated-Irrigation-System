// Included Libraries
#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp8266.h>
#include <EEPROM.h>

// key and wifi parameters
char ssid[] = "Fixbox-703786";
char pass[] = "ZjQ3NzMy";// Set password to "" for open networks.

//char ssid[] = "Ahmadi's Nova";
//char pass[] = "azerty123";// Set password to "" for open networks.
char auth[] = "yxKnFUEhArIlarzhBdEWbtArOj3dyMSE"; //char auth[] = "DCdZQBnP-jNF16I7kd_kp-v4Jhg_2mZE";  //App

// Labels
#define BLYNK_PRINT     Serial    // Comment this out to disable prints and save space
#define BLYNK_GREEN     "#00FF00"
#define BLYNK_BLUE      "#0000FF"
#define BLYNK_YELLOW    "#FFFF00"
#define BLYNK_RED       "#FF0000"
#define BLYNK_BLACK     "#000000"
#define BLYNK_WHITE     "#FFFFFF"

#define Mode_Vpin       V0
#define POMPERemp_Vpin  V1
#define POMPEIrrig_Vpin V2
#define Vanne_Vpin      V3
#define NIVEAU_Vpin     V4
#define Terminal_Vpin   V5
#define Menu_Vpin       V6
#define Button_Vpin     V8
#define Slider_Vpin     V9

WidgetTerminal  terminal(Terminal_Vpin);
BlynkTimer timer;
WiFiUDP Udp;

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";
const int timeZone = 1;     // Central European Time
unsigned int localPort = 8888;  // local port to listen for UDP packets
time_t getNtpTime();
void sendNTPpacket(IPAddress &address);

// Vars
byte    Mode, Menu, POMPERemp, POMPEIrrig, Vanne; // byte : [0..255]
unsigned int     Slider; // unsigned int [0..65535]
float   NIVEAU_inst, NIVEAU_low, NIVEAU_hi;
char s[80];

// EEPROM Adresses
const int EE_Mode = 0, EE_Menu = 1, EE_POMPERemp = 2, EE_POMPEIrrig = 3, EE_Vanne = 4, EE_Slider = 5, EE_NIVEAU_inst = 6, EE_NIVEAU_low = 10, EE_NIVEAU_hi = 14;

void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(Mode_Vpin, Mode);
  Blynk.virtualWrite(POMPERemp_Vpin, POMPERemp);
  Blynk.virtualWrite(POMPEIrrig_Vpin, POMPEIrrig);
  Blynk.virtualWrite(NIVEAU_Vpin, NIVEAU_inst);

  // Simulate the level
  if ((POMPERemp == 3) && (NIVEAU_inst < NIVEAU_hi)) {
    NIVEAU_inst = NIVEAU_inst + 1.5;
    EEPROM.put(EE_NIVEAU_inst, NIVEAU_inst);
  } else if ((POMPEIrrig == 3) && (NIVEAU_inst > NIVEAU_low)) {
    NIVEAU_inst = NIVEAU_inst - 2.2;
    EEPROM.put(EE_NIVEAU_inst, NIVEAU_inst);
  }

  // Limits tests
  if ((NIVEAU_inst >= NIVEAU_hi) && (POMPERemp >= 2)) {
    POMPERemp = 0;
    //    EEPROM.write(EE_POMPERemp,  0);
    if (Menu == 2) {
      Button_set(0, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "REMPLISSAGE", "ARRET");
    }
    snprintf(s, 80, "%02d/%02d/%04d %02d:%02d:%02d Fin Remplissage (niveau haut atteint)", day(), month(), year(), hour(), minute(), second());
    terminal.println(s);
  } else if ((NIVEAU_inst <= NIVEAU_low) && (POMPEIrrig >= 2)) {
    POMPEIrrig = 0;
    //    EEPROM.write(EE_POMPEIrrig, 0);
    if (Menu == 3) {
      Button_set(0, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "REMPLISSAGE", "ARRET");
    }
    snprintf(s, 80, "%02d/%02d/%04d %02d:%02d:%02d Arreter l'irrigation (niveau bas atteint)", day(), month(), year(), hour(), minute(), second());
    terminal.println(s);
  }

  if (Mode == 1) {
    if (NIVEAU_inst >= NIVEAU_hi) {
      set_POMPEIrrig_on();
      //      EEPROM.write(EE_POMPEIrrig, 3);
      snprintf(s, 80, "%02d/%02d/%04d %02d:%02d:%02d Démarrage Automatique de l'Irrigation", day(), month(), year(), hour(), minute(), second());
      terminal.println(s);
    } else if (NIVEAU_inst <= NIVEAU_low) {
      POMPERemp = 1;
      Blynk.virtualWrite(POMPERemp_Vpin, POMPERemp);
      Button_set(1, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "REMPLISSAGE prog", "ARRET");
      int timer_POMPERemp_on = timer.setTimeout(1000, set_POMPERemp_on);
      //      EEPROM.write(EE_POMPERemp,  3);
      snprintf(s, 80, "%02d/%02d/%04d %02d:%02d:%02d Démarrage Automatique du Remplissage", day(), month(), year(), hour(), minute(), second());
      terminal.println(s);
    }
  }

  terminal.flush();
  EEPROM.commit();    //Store data to EEPROM
}

BLYNK_WRITE(Terminal_Vpin) // Terminal
{
  String teminal_str;
  teminal_str = param.asStr();

  if (String("clear") == param.asStr()) {
    terminal.clear();   // Erase all values in the terminal
  } else if (String ("levels") == param.asStr()) {
    snprintf(s, 51, " LOW: %.2f cm \t Inst: %.2f cm \t Hi: %.2f cm", NIVEAU_low, NIVEAU_inst, NIVEAU_hi);
    terminal.println(s);

    //  } else if (String ("NIVEAU B") == param.asStr()) {
    //    char s[4];
    //
    //  } else if (String ("NIVEAU H") == param.asStr()) {
    //    char s[4];

  } else if (teminal_str.startsWith(String("set low"))) {
    teminal_str = teminal_str.substring(8);
    float terminal_f = String(teminal_str).toFloat();
    if (terminal_f < NIVEAU_hi) {
      NIVEAU_low = terminal_f;
      EEPROM.put(EE_NIVEAU_low, NIVEAU_low);
      snprintf(s, 35, " Low level was set to %.2f cm", NIVEAU_low);
      terminal.println(s);
    } else {
      terminal.println("low level must be less than HI level!");
      snprintf(s, 35, " HI level : %.2f cm", NIVEAU_hi);
      terminal.println(s);
    }
  } else if (teminal_str.startsWith(String("set hi"))) {
    teminal_str = teminal_str.substring(7);
    float terminal_f = String(teminal_str).toFloat();
    if (terminal_f > NIVEAU_low) {
      NIVEAU_hi = terminal_f;
      EEPROM.put(EE_NIVEAU_hi, NIVEAU_hi);
      snprintf(s, 35, " HI level was set to %.2f cm", NIVEAU_hi);
      terminal.println(s);
    } else {
      terminal.print("HI level must be greater than low level! ");
      snprintf(s, 35, " low level : %.2f cm", NIVEAU_low);
      terminal.println(s);
    }
  } else if (String ("help") == param.asStr()) {
    terminal.println("clear");
    terminal.println("levels");
    //    terminal.println("NIVEAU B");
    //    terminal.println("NIVEAU H");
    terminal.println("set low x.xx");
    terminal.println("set hi x.xx");

  } else {
    terminal.println("What? ما فهمتكش");
  }
  terminal.flush();
  EEPROM.commit();    //Store data to EEPROM
}

BLYNK_WRITE(Menu_Vpin) // Menu
{
  switch (param.asInt())
  {
    case 1: { // Mode
        Menu = 1;
        if (Mode == 0 ) { //"Manual"
          Button_set(0, BLYNK_BLACK, BLYNK_RED, BLYNK_BLACK , BLYNK_WHITE, "Auto", "Manual");
          Blynk.setProperty(Menu_Vpin, "labels", "Mode", "Pompe de Remplissage", "Pompe d'Irrigation", "Vanne");
        }
        else { //"Auto"
          Button_set(1, BLYNK_BLACK, BLYNK_RED, BLYNK_BLACK , BLYNK_WHITE, "Auto", "Manual");
          Blynk.setProperty(Menu_Vpin, "labels", "Mode");
        }
        break;
      }
    case 2: { // Pompe de Remplissage
        Menu = 2;
        if (POMPERemp < 2) {
          Button_set(0, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "REMPLISSAGE", "ARRET");
        }
        else {
          //POMPERemp = 2/3;
          Button_set(1, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "REMPLISSAGE", "ARRET");
        }
        break;
      }
    case 3: { // Pompe d'Irrigation
        Menu = 3;
        if (POMPEIrrig < 2) {
          Button_set(0, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "IRRIGATION", "ARRET");
        }
        else {
          //POMPEIrrig = 2/3;
          Button_set(1, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "IRRIGATION", "ARRET");
        }
        break;
      }
    case 4: { // Vanne
        Menu = 4;
        if (Vanne == 0) {
          Button_set(0, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "ON", "OFF");
        }
        else {
          //Vanne = 1;
          Button_set(1, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "ON", "OFF");
        }
        break;
      }
  }
}

BLYNK_WRITE(Button_Vpin) // Button
{
  int pinValue = param.asInt();
  switch (Menu) {
    case 1: { // Mode
        if (pinValue == 0) {
          Mode = 0;
          //          EEPROM.write(EE_Mode, 0);
          Blynk.virtualWrite(Mode_Vpin, 0);
          Button_set(0, BLYNK_BLACK, BLYNK_RED, BLYNK_BLACK , BLYNK_WHITE, "Auto", "Manual");
          Blynk.setProperty(Menu_Vpin, "labels", "Mode", "Pompe de Remplissage", "Pompe d'Irrigation", "Vanne");
          snprintf(s, 80, "%02d/%02d/%04d %02d:%02d:%02d Le mode Manuel est choisi", day(), month(), year(), hour(), minute(), second());
          terminal.println(s);
        }
        else {
          Mode = 1;
          //          EEPROM.write(EE_Mode, 1);
          Blynk.virtualWrite(Mode_Vpin, 1);
          Button_set(1, BLYNK_BLACK, BLYNK_RED, BLYNK_BLACK , BLYNK_WHITE, "Auto", "Manual");
          Blynk.setProperty(Menu_Vpin, "labels", "Mode");
          snprintf(s, 80, "%02d/%02d/%04d %02d:%02d:%02d Le mode Auto est choisi", day(), month(), year(), hour(), minute(), second());
          terminal.println(s);
        }
        break;
      }
    case 2: { // Pompe de Remplissage
        if (pinValue == 0) {
          POMPERemp = 0;
          //          EEPROM.write(EE_POMPERemp,  0);
          Blynk.virtualWrite(POMPERemp_Vpin, 0);
          Button_set(0, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "REMPLISSAGE", "ARRET");
          snprintf(s, 80, "%02d/%02d/%04d %02d:%02d:%02d Arret manuel du Remplissage", day(), month(), year(), hour(), minute(), second());
          terminal.println(s);
        }
        else {
          POMPERemp = 1;
          Blynk.virtualWrite(POMPERemp_Vpin, POMPERemp);
          Button_set(1, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "REMPLISSAGE prog", "ARRET");
          int timer_POMPERemp_on = timer.setTimeout(10000, set_POMPERemp_on);
          //          EEPROM.write(EE_POMPERemp,  3);
          snprintf(s, 80, "%02d/%02d/%04d %02d:%02d:%02d Démarrage manuel du Remplissage", day(), month(), year(), hour(), minute(), second());
          terminal.println(s);
        }
        break;
      }
    case 3: { // Pompe d'Irrigation
        if (pinValue == 0) {
          POMPEIrrig = 0;
          //          EEPROM.write(EE_POMPEIrrig, 0);
          Blynk.virtualWrite(POMPEIrrig_Vpin, 0);
          Button_set(0, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "IRRIGATION", "ARRET");
          snprintf(s, 80, "%02d/%02d/%04d %02d:%02d:%02d Arret manuel de l'Irrigation", day(), month(), year(), hour(), minute(), second());
          terminal.println(s);
        }
        else {
          set_POMPEIrrig_on();
          //          EEPROM.write(EE_POMPEIrrig, 3);
          Blynk.virtualWrite(POMPEIrrig_Vpin, 3);
          Button_set(1, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "IRRIGATION", "ARRET");
          snprintf(s, 80, "%02d/%02d/%04d %02d:%02d:%02d Démarrage manuel de l'Irrigation", day(), month(), year(), hour(), minute(), second());
          terminal.println(s);
        }
        break;
      }
    case 4: { // Vanne
        if (pinValue == 0) {
          Vanne = 0;
          //          EEPROM.write(EE_Vanne, 0);
          Blynk.virtualWrite(Vanne_Vpin, 0);
          Button_set(0, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "ON", "OFF");
          snprintf(s, 80, "%02d/%02d/%04d %02d:%02d:%02d Ferméture manuelle de la vanne", day(), month(), year(), hour(), minute(), second());
          terminal.println(s);
        }
        else {
          Vanne = 1;
          //          EEPROM.write(EE_Vanne, 1);
          Blynk.virtualWrite(Vanne_Vpin, 1);
          Button_set(1, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "ON", "OFF");
          snprintf(s, 80, "%02d/%02d/%04d %02d:%02d:%02d Ouverture manuelle de la vanne", day(), month(), year(), hour(), minute(), second());
          terminal.println(s);
        }
        break;
      }
  }
  EEPROM.commit();    //Store data to EEPROM
}

BLYNK_WRITE(Slider_Vpin) // Slider
{
  Slider = param.asInt(); // assigning incoming value from pin POMPERemp_Vpin to a variable
}

void set_level(float l_level, float h_level)
{
  NIVEAU_low  = l_level;
  //  EEPROM.put(EE_NIVEAU_low, NIVEAU_low);
  NIVEAU_hi   = h_level;
  //  EEPROM.put(EE_NIVEAU_hi, NIVEAU_hi);
  //  EEPROM.commit();    //Store data to EEPROM
}

void display_5()
{
  snprintf(s, 80, "%02d/%02d/%04d %02d:%02d:%02d", day(), month(), year(), hour(), minute(), second());
  terminal.println(s);
  terminal.println("\t Repeated every 5s");
}

void display_10()
{
  snprintf(s, 80, "%02d/%02d/%04d %02d:%02d:%02d", day(), month(), year(), hour(), minute(), second());
  terminal.println(s);
  terminal.println("\t Repeated every 10s");
}

void set_POMPERemp_on() {
  POMPERemp = 3;
  Blynk.virtualWrite(POMPERemp_Vpin, POMPERemp);
  Button_set(1, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "REMPLISSAGE", "ARRET");
}

void set_POMPEIrrig_on() {
  POMPEIrrig = 3;
  Blynk.virtualWrite(POMPEIrrig_Vpin, POMPEIrrig);
  Button_set(1, BLYNK_BLUE, BLYNK_GREEN, BLYNK_BLACK, BLYNK_RED, "IRRIGATION", "ARRET");
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  Udp.begin(localPort); // "Starting UDP"
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
  EEPROM.begin(512);  //Initialize EEPROM

  timer.setInterval(2000L, myTimerEvent);
  //  timer.setInterval( L, display_5);
  //  timer.setInterval(10000L, display_10);

  terminal.println("بسم اللّه الرّحمان الرّحيم");
  terminal.println("وجعلنا من الماء كلّ شيء حيّ");
  terminal.println("صدق اللّه العظيم");

  snprintf(s, 35, "%02d/%02d/%04d %02d:%02d:%02d Rebooted", day(), month(), year(), hour(), minute(), second());
  terminal.println(s);
  terminal.flush();
  // Software Intialize state
  Mode = 0;
  Menu = 1;
  POMPERemp = 0;
  POMPEIrrig = 0;
  Vanne = 0;
  Slider = 0;
  //  NIVEAU_inst = 0;
  //  NIVEAU_low = 0.;
  //  NIVEAU_hi = 100.;

  //Initialize state from EEPROM
  //  Mode = EEPROM.read(EE_Mode);
  //  Menu = EEPROM.read(EE_Menu);
  //  POMPERemp = EEPROM.read(EE_POMPERemp);
  //  POMPEIrrig = EEPROM.read(EE_POMPEIrrig);
  //  Vanne = EEPROM.read(EE_Vanne);
  //  Slider = EEPROM.read(EE_Slider);

  EEPROM.get(EE_NIVEAU_inst, NIVEAU_inst);
  EEPROM.get(EE_NIVEAU_low, NIVEAU_low);
  EEPROM.get(EE_NIVEAU_hi, NIVEAU_hi);

  Blynk.virtualWrite(Menu_Vpin, Menu);
  Blynk.virtualWrite(Slider_Vpin, 0);
  if (Mode == 0) { // "Manual"
    Button_set(0, BLYNK_BLACK, BLYNK_RED, BLYNK_BLACK , BLYNK_WHITE, "Auto", "Manual");
    Blynk.setProperty(Menu_Vpin, "labels", "Mode", "Pompe du Remplissage", "Pompe d'Irrigation", "Vanne");
  }
  else {// "Auto"
    Button_set(1, BLYNK_BLACK, BLYNK_RED, BLYNK_BLACK , BLYNK_WHITE, "Auto", "Manual");
    Blynk.setProperty(Menu_Vpin, "labels", "Mode");
  }
  EEPROM.commit();    //Store data to EEPROM
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}

void Button_set(byte b_state, String b_On_color, String b_On_Bcolor, String b_Off_color, String b_Off_Bcolor, String b_off, String b_on)
{
  Blynk.virtualWrite(Button_Vpin, b_state);
  Blynk.setProperty(Button_Vpin, "onColor", b_On_color);
  Blynk.setProperty(Button_Vpin, "onBackColor", b_On_Bcolor);
  Blynk.setProperty(Button_Vpin, "offColor", b_Off_color);
  Blynk.setProperty(Button_Vpin, "offBackColor", b_Off_Bcolor);
  Blynk.setProperty(Button_Vpin, "onLabel", b_off);
  Blynk.setProperty(Button_Vpin, "offLabel", b_on);
}
/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
