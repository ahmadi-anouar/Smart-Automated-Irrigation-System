/************************************************
                 Amra project
 ************************************************
    Last update code: 19/12/2019
    written by Ibtihel AHMADI
    collaborated with Anouar AHMADI
    Arduino nano ATmega328P
    _____________________________________________
    RF Communication : HC-12
    2 Relays shild
    ---------------------------------------------
    Mesuring distance in cm
    Relay controlling
    _____________________________________________
    To do list:
    - What to do if no answer to 'N'?
    - Add the command of irrigation pump and the
    electronic solenoid valve
    - Change the pin of relay to be different of
    the Led pin
    - Initiate variables spetialy that refers to
    pumps and solenoid valve
    - The Irregation pump should be commended by RF
    _____________________________________________
    V1.01: 20/12/2019 by Anouar
     -Adding the Irrigation pump for test in pin 2
     ____________________________________________
    V1.00: 19/12/2019
     - WIFI connecting
     - Getting level in cm from RF
     - Publishing level via MQTT and Serial
     connection
     - Getting status of POMPERemp from MQTT
     and updating it      
 ********************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

// Update these with values suitable for your network.
const char* ssid        = "Fixbox-703786";
const char* password    = "ZjQ3NzMy";
const char* mqtt_server = "192.168.0.17";

SoftwareSerial HC12(4, 5); // HC-12 TX Pin, HC-12 RX Pin
WiFiClient     espClient;
PubSubClient   client(espClient);

long    lastMsg = 0;
int     i, flag, value = 0;
char    msg[50];
String  topic_str;

int POMPERemp=0;
int POMPEIrrig=2;



void setup_wifi() {
  // We start by connecting to a WiFi network
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  String topic_str(topic);              // Convert char* to string
  
  if (topic_str == "POMPERemp") {       // If it talk about POMPERemp
    if ((char)payload[0] == '1') {
      digitalWrite(POMPERemp, LOW);   // Turn the Pump on (it is active low on the ESP-01)
    } else {
      digitalWrite(POMPERemp, HIGH);  // Turn the Pump off by making the voltage HIGH
    }
  }  

  if (topic_str == "POMPEIrrig") {       // If it talk about POMPERemp
    if ((char)payload[0] == '1') {
      digitalWrite(POMPEIrrig, LOW);   // Turn the Pump on (it is active low on the ESP-01)
    } else {
      digitalWrite(POMPEIrrig, HIGH);  // Turn the Pump off by making the voltage HIGH
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("MESSAGE", "hello world");
      // ... and resubscribe
      client.subscribe("POMPERemp");
      client.subscribe("POMPEIrrig");
      client.subscribe("NIVEAU");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 10 seconds");
      // Wait 5 seconds before retrying
      delay(10000);
    }
  }
}

void setup() {
  pinMode(POMPERemp, OUTPUT);     // Initialize the POMPERemp pin as an output
  pinMode(POMPEIrrig, OUTPUT);     // Initialize the POMPERemp pin as an output
  Serial.begin(9600);
  HC12.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  long now = millis();                            // What time is it?
  if (now - lastMsg > 4000) {                     // passed 4 seconds?
    lastMsg = now; flag = 0; i = 0;               // if yes, init vars
    HC12.write('N');                              // Ask for level

    while (HC12.available() && flag == 0) {       // If HC-12 has data not been read yet
      while (HC12.available()) {                  // read it
        msg[i] = char(HC12.read());               // as string
        if ((int(msg[i]) == 10) || (i == 49)) {
          flag = 1;
        }
        i = i + 1;
      }
      client.publish("NIVEAU", msg);              // then publish it via MQTT
    }
  }
}
