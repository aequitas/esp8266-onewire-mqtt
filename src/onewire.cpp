#include "onewire.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "wifi.h"

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

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

void addr_to_str(byte *addr, char *str){
    snprintf(str, 17, "%02x%02x%02x%02x%02x%02x%02x%02x",
    addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7]);
}

// read sensors into metrics array
void read_sensors(Metric metrics[]){
    int numberOfDevices = getSensors();
    unsigned long start_time, stop_time;
    DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address
    char tmp_addr_str[17];

    // record time before start reading sensors
    start_time = millis();
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
    // record time when reading sensors has finished
    stop_time = millis();

    // clear rest of devices in metrics list
    for (int i=numberOfDevices;i<SENSOR_LIMIT;i++){
        strcpy(metrics[i].name, "");
    }
    // add sensor read runtime
    strcpy(metrics[numberOfDevices].name, get_mac_addr());
    strcpy(metrics[numberOfDevices].type, "runtime");
    metrics[numberOfDevices].value = (float)(stop_time - start_time);
}
