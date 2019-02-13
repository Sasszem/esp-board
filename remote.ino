#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <LuaWrapper.h>
#include "shift.hpp"
#include "matrix.hpp"
#include "tasks.hpp"

TaskManager TM;

#include "network.hpp"

Network network;
ShiftRegister shift(16,15);
Matrix matrix(D5, 12, D7, &shift);
//LiquidCrystal lcd(D3, &shift);

bool state = false;
void led_task()
{
  //digitalWrite(LED_BUILTIN, state);
  //state = !state;
  digitalWrite(LED_BUILTIN, !matrix.state[0][1]);
}

void setLCD_task()
{
  if (matrix.state[0][0])
  {
    shift.set(0);
    Serial.println("LCD OFF");
  }
  if (matrix.state[0][1])
  {
    shift.set(255);
    Serial.println("LCD_ON");
  }
}
void updateMatrix_task()
{
  matrix.update();
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.begin(115200);
  SPIFFS.begin();
  Serial.println("\n");
  Serial.println("Begin booting...");
  network.load_aps();

  unsigned int t = millis();

  TM.add_task(&led_task, 150, t);
  TM.add_task(&updateMatrix_task, 50, t);
  TM.add_task(&setLCD_task, 100, t);
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

