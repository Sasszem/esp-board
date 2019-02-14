#pragma once
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <LuaWrapper.h>
#include "tasks.hpp"
#include "IO.h"
TaskManager TM;
IO io;
#include "network.hpp"
Network network;

bool state = false;
void led_task()
{
  //digitalWrite(LED_BUILTIN, state);
  //state = !state;
  digitalWrite(LED_BUILTIN, !io.matrix.state[0][1]);
}

void setLCD_task()
{
  if (io.matrix.state[0][0])
  {
    io.lcd.noBacklight();
    Serial.println("LCD OFF");
  }
  if (io.matrix.state[0][1])
  {
    io.lcd.backlight();
    Serial.println("LCD_ON");
  }
}
void updateIO_task()
{
  io.update();
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.begin(115200);
  SPIFFS.begin();
  Serial.println("\n");
  Serial.println("Begin booting...");
  io.lcd.begin(16,2);
  
  //lcd.blink();
  //lcd.cursor();
  network.load_aps();

  unsigned int t = millis();

  TM.add_task(&led_task, 150, t);
  TM.add_task(&updateIO_task, 50, t);
  TM.add_task(&setLCD_task, 100, t);
  io.lcd.print("hello, world!");
}



void loop() {
  int t = millis();
  TM.update(t);
  
  // put your main code here, to run repeatedly:
  network.run();
  /*
  Serial.println("Matrix state: ");
  for (int i = 0; i<4; i++)
  {
    for (int j = 0; j<3; j++)
    {
      Serial.print(matrix.state[i][j]);
      Serial.write(" ");
    }
    Serial.println();
  }*/
}

