#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Wifi name
#define WLAN_SSID "Your_WiFi_Name"
// Wifi password
#define WLAN_PASS "Your_WiFi_Password"

// setup Adafruit
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
// fill your username
#define AIO_USERNAME "Your_Adafruit_Username"
// fill your key
#define AIO_KEY "Your_Adafruit_Key"

// setup MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// setup publish
Adafruit_MQTT_Publish light_pub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/light");
Adafruit_MQTT_Publish temp_pub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/lab8-temp");

// setup subcribe
Adafruit_MQTT_Subscribe light_sub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light", MQTT_QOS_1);

int led_counter = 0;
int led_status = HIGH;
unsigned long lastPing = 0;

String tempBuffer = "";

void lightcallback(char *value, uint16_t len)
{
  if (value[0] == '0')
    Serial.print('a');
  if (value[0] == '1')
    Serial.print('A');
}

void setup()
{
  // put your setup code here, to run once:
  // set pin 2,5 as OUTPUT
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  // set busy pin HIGH
  digitalWrite(5, HIGH);

  Serial.begin(115200);

  // connect Wifi
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  // subscribe light feed
  light_sub.setCallback(lightcallback);
  mqtt.subscribe(&light_sub);

  // connect MQTT
  while (mqtt.connect() != 0)
  {
    mqtt.disconnect();
    delay(500);
  }

  // finish setup, set busy pin LOW
  digitalWrite(5, LOW);
}

void loop()
{
  // put your main code here, to run repeatedly:

  // Check MQTT connection and reconnect if needed
  if (!mqtt.connected())
  {
    digitalWrite(5, HIGH); // Set busy when reconnecting
    while (mqtt.connect() != 0)
    {
      mqtt.disconnect();
      delay(500);
    }
    digitalWrite(5, LOW);
  }

  // MQTT ping to keep connection alive
  if (millis() - lastPing > 30000)
  {
    mqtt.ping();
    lastPing = millis();
  }

  // receive packet
  mqtt.processPackets(30);

  // read serial
  while (Serial.available())
  {
    int msg = Serial.read();

    if (msg == 'o')
    {
      Serial.print('O');
    }
    else if (msg == 'a')
    {
      light_pub.publish(0);
    }
    else if (msg == 'A')
    {
      light_pub.publish(1);
    }
    else if (msg == '!')
    {
      // Start of temperature message
      tempBuffer = "!";
    }
    else if (msg == '#')
    {
      // End of temperature message
      tempBuffer += "#";
      // Parse temperature: !TEMP:25.5#
      if (tempBuffer.startsWith("!TEMP:") && tempBuffer.endsWith("#"))
      {
        int colonIndex = tempBuffer.indexOf(':');
        int hashIndex = tempBuffer.indexOf('#');
        String tempValue = tempBuffer.substring(colonIndex + 1, hashIndex);
        float temperature = tempValue.toFloat();

        // Publish to Adafruit with retry
        int retry = 0;
        while (retry < 3)
        {
          if (temp_pub.publish(temperature))
          {
            // Success - blink LED to confirm
            digitalWrite(2, LOW);
            delay(100);
            digitalWrite(2, HIGH);
            break;
          }
          retry++;
          delay(100);
        }
      }
      tempBuffer = "";
    }
    else if (tempBuffer.length() > 0)
    {
      // Building temperature message
      tempBuffer += (char)msg;
    }
  }

  led_counter++;
  if (led_counter == 100)
  {
    // every 1s
    led_counter = 0;
    // toggle LED
    if (led_status == HIGH)
      led_status = LOW;
    else
      led_status = HIGH;

    digitalWrite(2, led_status);
  }
  delay(10);
}
