#include <Thread.h>
#include <ThreadController.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <ESP8266Wifi.h>
#include "wifi.h"
#include "mqtt.h"

// define threads
ThreadController controller = ThreadController();
Thread thr_sensor_read = Thread();
Thread thr_mqtt_publish = Thread();

// sensors
#define ONE_WIRE_BUS 2 // onewire bus gpio pin
#define TEMPERATURE_PRECISION 9 // Lower resolution
#define SENSOR_LIMIT 32 // maximum count of sensors to support

#define MQTT_SERVER "mqtt"

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// create global metrics table
#define METRIC_COUNT SENSOR_LIMIT

struct Metric {
    char type[16];
    char name[32];
    float value;
} metrics[METRIC_COUNT];

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

void setup_sensors(void){
    // set pullup for onewire bus port
    pinMode(2, INPUT_PULLUP);

    // Start up the library
    sensors.begin();

    // report parasite power requirements
    Serial.print("Parasite power is: ");
    if (sensors.isParasitePowerMode()){
        Serial.println("ON");
    } else {
        Serial.println("OFF");
    }
}

int getSensors(void){
    int numberOfDevices;

    // Reset known devices
    sensors.begin();

    // Grab a count of devices on the wire
    numberOfDevices = sensors.getDeviceCount();

    // locate devices on the bus
    Serial.print("Locating devices...");

    Serial.print("Found ");
    Serial.print(numberOfDevices, DEC);
    Serial.println(" devices.");

    return numberOfDevices;
}

void printTemperature(DeviceAddress deviceAddress)
{
    float tempC = sensors.getTempC(deviceAddress);
    Serial.print("Temp C: ");
    Serial.println(tempC);
}

void addr_to_str(byte *addr, char *str){
    snprintf(str, 17, "%02x%02x%02x%02x%02x%02x%02x%02x",
    addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7]);
}

void sensor_read(void){
    int numberOfDevices = getSensors();
    DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address
    char tmp_addr_str[17];

    // call sensors.requestTemperatures() to issue a global temperature
    // request to all devices on the bus
    sensors.requestTemperatures(); // Send the command to get temperatures

    // Loop through each device, print out temperature data
    for(int i=0;i<numberOfDevices; i++)
    {
        // Search the wire for address
        if(sensors.getAddress(tempDeviceAddress, i))
        {
            // get string value for sensor address
            addr_to_str(tempDeviceAddress, tmp_addr_str);
            // assign to metric name
            strcpy(metrics[i].name, tmp_addr_str);
            strcpy(metrics[i].type, "temp");
            metrics[i].value = sensors.getTempC(tempDeviceAddress);
        } else {
            Serial.print("Found ghost device at ");
            Serial.print(i, DEC);
            Serial.println(" but could not detect address. Check power and cabling");
            strcpy(metrics[i].name, "");
        }
    }
}

void mqtt_publish(void){
    for (int i=0;i<METRIC_COUNT;i++){
        if (metrics[i].name[0]){
            send_metric(mqtt_client, metrics[i].name, metrics[i].type, metrics[i].value);
        }
    }
}

bool connect(void){
    // ensure connection to wifi
    connect_wifi();

    // ensure connection to mqtt
    connect_mqtt(mqtt_client);
}

void setup(void){
    Serial.begin(9600);

    setup_wifi();

    // set mqtt client name and server
    char mac_str[18];
    get_mac_addr(mac_str);
    setup_mqtt(mqtt_client, mac_str);
    mqtt_client.setServer(MQTT_SERVER, 1883);

    setup_sensors();

    // read sensors every 30 seconds
    thr_sensor_read.onRun(sensor_read);
    thr_sensor_read.setInterval(5*1000);

    // publish metrics to mqtt every 10 seconds
    thr_mqtt_publish.onRun(mqtt_publish);
    thr_mqtt_publish.setInterval(10*1000);

    // add threads to controller
    controller.add(&thr_sensor_read);
    controller.add(&thr_mqtt_publish);

    Serial.println("setup");
}

void loop(void){
    // ensure connection to wifi and mqtt
    connect();

    // have controller check threads which need to be run
    controller.run();
}
