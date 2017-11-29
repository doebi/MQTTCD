#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <libnotify/notify.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://mqtt.devlol.org:1883"
#define CLIENTID    "user@notebook"
#define TOPIC       "doebi/notify/#"
#define QOS         0
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;
void mqtt_connect();
