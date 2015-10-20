#include <ESP8266WiFi.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

void get_mac_addr(char *str){
    uint8_t mac[6];
    WiFi.macAddress(mac);

    snprintf(str, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

bool wifi_connected(){
    return WiFi.status() == WL_CONNECTED;
}

void setup_wifi(void){
    delay(10);

    WiFi.begin(ssid, password);
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

    Serial.print("MAC address: ");
    char mac_str[18];
    get_mac_addr(mac_str);
    Serial.println(mac_str);

    return true;
}
