#include "wifi.h"
#include <ESP8266WiFi.h>

char mac_str[18];

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

char * get_mac_addr(void){
    return mac_str;
}

bool wifi_connected(){
    return WiFi.status() == WL_CONNECTED;
}

void setup_wifi(void){
    delay(10);

    WiFi.begin(ssid, password);

    // assign mac string to global variable
    uint8_t mac[6];
    WiFi.macAddress(mac);

    snprintf(mac_str, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);


    Serial.print("MAC address: ");
    Serial.println(mac_str);
}

bool connect_wifi(void){
    if (wifi_connected()){
        return true;
    }

    // We start by connecting to a WiFi network
    Serial.print("Connecting to WiFi ");
    Serial.println(ssid);

    while (! wifi_connected()) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    return true;
}
