#include "system.hpp"
#include "Arduino.h"


void espSystem::TaskManager::add_task(void (*task)(espSystem::System*), unsigned int interval, unsigned int t)
{
    Task new_task;
    new_task.interval = interval;
    new_task.task = task;
    new_task.target = t + interval;
    tasks.push_back(new_task);
    task_count++;
}

void espSystem::TaskManager::update(unsigned int t)
{
    for (int i = 0; i<task_count; i++)
    {
        if (t>=tasks[i].target)
        {
            tasks[i].task(parent);
            tasks[i].target += tasks[i].interval;
        }
    }
}

void espSystem::Network::load_aps()
{
  multi = new ESP8266WiFiMulti();
  if (SPIFFS.exists(filename))
  {
    File list = SPIFFS.open(filename, "r");
    while (list.available())
    {
      String ssid = list.readStringUntil('\n');
      String pwd = list.readStringUntil('\n');

      ssid.trim();
      pwd.trim();
      
      Serial.println("SSID: " + ssid);
      Serial.println("   PWD: " + pwd);
      if (ssid.length()!=0 && pwd.length()!=0)
        multi->addAP(ssid.c_str(), pwd.c_str());
    }
    list.close();
  }
  else
  {
    Serial.print("\"");
    Serial.print(filename);
    Serial.println("\" does not exists!");
  }
}

void espSystem::Network::run()
{
    if (cstate == CONNECTED)
    {
        if (multi->run()!=WL_CONNECTED)
        {
            cstate = CONNECTING;
            Serial.println("WiFi disconnected!");
        }
    }
    else if (cstate == CONNECTING)
    {
        if (multi->run()==WL_CONNECTED)
        {
            Serial.print("WiFi connected to ");
            Serial.println(WiFi.SSID());
            cstate = CONNECTED;
        }
    }
}


void espSystem::Network::set_ap(String ssid, String pwd)
{
    File old = SPIFFS.open(filename, "r");
    File nw = SPIFFS.open("/network_temp","w");
    if (old && nw)
    {
        bool done = false;
        while (old.available())
        {
            String id = old.readStringUntil('\n');
            String p = old.readStringUntil('\n');
            id.trim();
            p.trim();
            if (id.length()!=0)
            {
                if (id.equals(ssid))
                {   
                    done=true;
                    if (pwd.length()>0)
                    {
                      nw.println(id);
                      nw.println(pwd);
                    }
                }
                else
                {
                    nw.println(id);
                    nw.println(p);
                }
            }
        }
        if (!done && pwd.length()>0)
        {
            nw.println(ssid);
            nw.println(pwd);
        }
        nw.close();
        old.close();
        SPIFFS.remove(filename);
        SPIFFS.rename("/network_temp",filename);
    }
    delete multi;
    this->load_aps();
}


void updateIO_task(espSystem::System* sys) {
  sys->io.update();
}

espSystem::System::System() {
  TM.parent = this;
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.begin(74880);
  SPIFFS.begin();
  Serial.println("\n");
  Serial.println("Begin booting...");
  io.lcd.begin(16,2);

  SPIFFS.begin();
  
  network.load_aps();

  unsigned int t = millis();

  TM.add_task(&updateIO_task, 50, t);
  io.lcd.print("Welcome!");
}

void espSystem::System::update() {
  int t = millis();
  TM.update(t);
  
  network.run();
}
