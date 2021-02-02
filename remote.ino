#pragma once
#include <ESP8266HTTPClient.h>
#include "system.hpp"

espSystem::System sys;

void led_task(espSystem::System *sys)
{
  digitalWrite(LED_BUILTIN, !sys->io.matrix.state[0][1]);
}

void setLCD_task(espSystem::System* sys)
{
  if (sys->io.matrix.state[0][0])
  {
    sys->io.lcd.noBacklight();
    Serial.println("LCD OFF");
  }
  if (sys->io.matrix.state[0][1])
  {
    sys->io.lcd.backlight();
    Serial.println("LCD_ON");
  }
}

void sendRequest_task(espSystem::System* sys) {
  if (sys->io.matrix.state[0][2])
  {
    sys->io.lcd.clear();
    HTTPClient http;
    // Your Domain name with URL path or IP address with path
      http.begin("http://sassze.github.io");
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        sys->io.lcd.write(payload.c_str());
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        sys->io.lcd.write("Error: ");
        sys->io.lcd.write(httpResponseCode);
      }
      // Free resources
      http.end();
  }
}

void setup() {
  sys.TM.add_task(&led_task, 150, 100);
  sys.TM.add_task(&setLCD_task, 100, 100);
  sys.TM.add_task(&sendRequest_task, 100, 100);
}


void loop() {
  sys.update();
}
