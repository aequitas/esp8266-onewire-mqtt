#include <PubSubClient.h>
#include "wifi.h"

const char* mqtt_server = "mqtt";
char mqtt_name[16];

void setup_mqtt(PubSubClient client, char * name){
    // mqtt setup
    // client.setServer(mqtt_server, 1883);
    strcpy(mqtt_name, name);
}

bool connect_mqtt(PubSubClient client){
    if (!client.connected()) {
        Serial.print("Connecting to MQTT broker ");
        Serial.print(mqtt_server);
        Serial.print(" as ");
        Serial.println(mqtt_name);
        while (! client.connect(mqtt_name)) {
            Serial.print(".");
            delay(500);
        }
        Serial.println();
        Serial.println("MQTT connected");
    }
    return client.connected();
}

void send_metric(PubSubClient client, char *root, char *dev_id, char *type, char *value){
    char topic[80];
    bool success;

    // make sure mqtt is connected
    if (!client.connected()) {
        Serial.println("not connected");
        return;
    }

    snprintf(topic, 75, "%s/%s/%s", root, dev_id, type);
    success = client.publish(topic, value);

    Serial.print("Sent ");
    Serial.print(topic);
    Serial.print(" ");
    Serial.print(value);
    Serial.print(" ");
    if (success){
        Serial.println("succes");
    } else {
        Serial.println("failed");
    }
}

void send_metric(PubSubClient client, char *dev_id, char *type, long value){
    char str_value[10];
    itoa(value, str_value, 10);

    send_metric(client, (char *)"sensors", dev_id, type, str_value);
}

void send_metric(PubSubClient client, char *dev_id, char *type, float value){
    char str_value[10];
    dtostrf(value, 4, 2, str_value);

    send_metric(client, (char *)"sensors", dev_id, type, str_value);
}

// send uptime
void send_uptime(PubSubClient client){
    char str_value[10];

    itoa(millis()/1000, str_value, 10);
    send_metric(client, (char *)"devices", get_mac_addr(), (char *)"uptime", str_value);
}
