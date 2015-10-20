#include "metrics.h"

// sensors
#define ONE_WIRE_BUS 2 // onewire bus gpio pin
#define TEMPERATURE_PRECISION 9 // Lower resolution
#define SENSOR_LIMIT 32 // maximum count of sensors to support

void setup_sensors(void);
int getSensors(void);
void read_sensors(Metric[]);
