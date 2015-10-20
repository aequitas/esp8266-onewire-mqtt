#include <Thread.h>
#include <ThreadController.h>
#include <PubSubClient.h>
#include <ESP8266Wifi.h>
#include "wifi.h"
#include "mqtt.h"
#include "onewire.h"

// define threads
ThreadController controller = ThreadController();
Thread thr_sensor_read = Thread();
Thread thr_mqtt_publish = Thread();

#define MQTT_SERVER "mqtt"

// create global metrics table
#define METRIC_COUNT SENSOR_LIMIT
Metric metrics[METRIC_COUNT];

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

// mqtt publish thread
void mqtt_publish(void){
    // publish metrics to mqtt
    for (int i=0;i<METRIC_COUNT;i++){
        if (metrics[i].name[0]){
            send_metric(mqtt_client, metrics[i].name, metrics[i].type, metrics[i].value);
        }
    }
    // publish uptime
    send_uptime(mqtt_client);
}

// sensor read thread
void sensor_read(void){
    // read sensors into metrics struct.
    read_sensors(metrics);
}

// ensure connections to wifi and mqtt
bool connect(void){
    // ensure connection to wifi
    connect_wifi();

    // ensure connection to mqtt
    connect_mqtt(mqtt_client);
}

// configure threads, intervals and thread controller
void setup_threads(void){
    // read sensors every 30 seconds
    thr_sensor_read.onRun(sensor_read);
    thr_sensor_read.setInterval(5*1000);

    // publish metrics to mqtt every 10 seconds
    thr_mqtt_publish.onRun(mqtt_publish);
    thr_mqtt_publish.setInterval(10*1000);

    // add threads to controller
    controller.add(&thr_sensor_read);
    controller.add(&thr_mqtt_publish);
}

// perform all setup
void setup(void){
    Serial.begin(9600);

    setup_wifi();

    // set mqtt client name and server
    setup_mqtt(mqtt_client, get_mac_addr());
    mqtt_client.setServer(MQTT_SERVER, 1883);

    setup_sensors();

    setup_threads();

    Serial.println("setup");
}

// ensure connection and invoke thread controller run
void loop(void){
    // ensure connection to wifi and mqtt
    connect();

    // have controller check threads which need to be run
    controller.run();
}
