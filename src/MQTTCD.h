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
#define TOPIC       "#"
#define QOS         1
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    syslog(LOG_NOTICE, "Message with token value %d delivery confirmed", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    char* payloadptr = message->payload;
    char msg[message->payloadlen+1];
    strcpy(msg, payloadptr);
    msg[message->payloadlen] = '\0';

    syslog(LOG_NOTICE, "Message arrived");
    NotifyNotification * notification = notify_notification_new (topicName,
                                                                 msg,
                                                                 "dialog-information");
    notify_notification_show (notification, NULL);
    g_object_unref(G_OBJECT(notification));

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    syslog(LOG_NOTICE, "Connection lost: %s", cause);
}
