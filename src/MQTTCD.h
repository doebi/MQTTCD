#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <libnotify/notify.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://mqtt.devlol.org:1883"
#define CLIENTID    "user@notebook"
#define TOPIC       "doebi/notify"
#define QOS         1
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    syslog(LOG_NOTICE, "Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    char* payloadptr = message->payload;
    payloadptr[message->payloadlen+1] = '\0';

    syslog(LOG_NOTICE, "Message arrived\n");
    NotifyNotification * msg = notify_notification_new ("MQTT Notification",
                                                        payloadptr,
                                                        "dialog-information");
    notify_notification_show (msg, NULL);
    g_object_unref(G_OBJECT(msg));

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    syslog(LOG_NOTICE, "\nConnection lost\n");
    syslog(LOG_NOTICE, "     cause: %s\n", cause);
}
