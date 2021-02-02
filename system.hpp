#include <FS.h>
#include "hardware.hpp"
#include <vector>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

namespace espSystem {
    class System;
  
    typedef struct {
        void (*task)(System*);
        unsigned int interval;
        unsigned int target;
    } Task;

    class TaskManager
    {
        public:
            std::vector<Task> tasks;
            System* parent;
            void update(unsigned int t);
            void add_task(void (*task)(System*), unsigned int interval, unsigned int t); 
            int task_count=0;
    };

    enum NetworkConnectionState {CONNECTED, CONNECTING};

    class Network
    {
        private:
            const char filename[7] = "/ssids";
            ESP8266WiFiMulti *multi;
            NetworkConnectionState cstate = CONNECTING;
        public:
            void load_aps();
            void set_ap(String ssid, String pwd);
            void del_ap(String ssid);
            void run();
    };
    
    class System {
        public:
            System();
            void update();
            board::IO io;
            TaskManager TM;
            Network network;
    };
}
