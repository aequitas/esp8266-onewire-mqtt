#include <PubSubClient.h>

void setup_mqtt(PubSubClient, char *);
bool connect_mqtt(PubSubClient);
void send_metric(PubSubClient, char *, char *, char *, char *);
void send_metric(PubSubClient, char *, char *, long);
void send_metric(PubSubClient, char *, char *, float);
void send_uptime(PubSubClient);
