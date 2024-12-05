#ifndef __TRIGGER_WIFI_H__
#define __TRIGGER_WIFI_H__

#include <WiFi.h>
#include <WiFiClientSecure.h>

WiFiClientSecure client;

#define Debug_Serial_Mon

void WIFI_Connect()
{
  int i = 0;
  	
  while (WiFi.status() != WL_CONNECTED) {
    
    #ifdef Debug_Serial_Mon
    	Serial.print("Trying to connect to ");
      Serial.println(wifi_ssids[i]);
    #endif
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssids[i], wifi_passwords[i]);
    
    int retry_count = 0;
    while (WiFi.status() != WL_CONNECTED && retry_count < 30) // 15 sec tentando																
    {
      delay(250);
      Serial.print(".");
      digitalWrite(LEDpin, HIGH);
      delay(250);
      digitalWrite(LEDpin, LOW);
      retry_count++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      #ifdef Debug_Serial_Mon
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
      #endif
      return;
    }

    i = (i + 1) % max_networks; // Avança para a próxima rede ou reinicia para a primeira
  }
}

#endif

