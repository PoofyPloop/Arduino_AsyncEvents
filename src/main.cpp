// External LED Wiring, PIR Sensor Test Program
//
// @author PoofyPloop

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

char ssid[] = SECRET_SSID;      // your network SSID (name)
char pass[] = SECRET_SSID_PASS; // your network password
int keyIndex = 0;               // your network key Index number (needed only for WEP)
WiFiClient client;

int LEDState = HIGH;

unsigned long myChannelNumber = SECRET_CH_ID;
const char *myWriteAPIKey = SECRET_API_KEY;

// pin assignments for external LEDs
#define PIN_LED_GREEN D1
#define PIN_LED_YELLOW D2
#define PIN_LED_RED D3

// pin assignment for PIR input
#define PIN_PIR D5

volatile bool bEventOccurred;

void IRAM_ATTR isr()
{
  bEventOccurred = true;
}

void setup()
{
  // setup LED outputs
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);

  pinMode(D4, OUTPUT);
  digitalWrite(D4, LEDState);

  // setup PIR input
  pinMode(PIN_PIR, INPUT);

  // built-in LED
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client); // Initialize ThingSpeak

  attachInterrupt(digitalPinToInterrupt(PIN_PIR), isr, CHANGE);

  Serial.println("\nAsync Events");
}

void loop()
{
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      digitalWrite(PIN_LED_YELLOW, HIGH);
      delay(5000);
    }
    Serial.println("\nConnected.");
    digitalWrite(PIN_LED_YELLOW, LOW);
    digitalWrite(PIN_LED_GREEN, HIGH);
    // print ip address
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
  }

  if (bEventOccurred)
  {
    int time = 2;
    while (time > 0)
    {
      time--;
      digitalWrite(PIN_LED_RED, HIGH);
      delay(1000);
    }
    int x = ThingSpeak.writeField(myChannelNumber, 1, 1, myWriteAPIKey);
    if (x == 200)
    {
      Serial.println("Channel update successful.");
    }
    else
    {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    digitalWrite(PIN_LED_RED, LOW);
    bEventOccurred = false;
  }
}
