#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

enum ConnectionState {CONNECTED, CONNECTING};

class Network
{
    private:
        const char filename[7] = "/ssids";
        ESP8266WiFiMulti *multi;
        ConnectionState cstate = CONNECTING;
    public:
        void load_aps();
        void set_ap(String ssid, String pwd);
        void del_ap(String ssid);
        void run();
};
