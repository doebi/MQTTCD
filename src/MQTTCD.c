#include "MQTTCD.h"

void ondelivered(void *context, MQTTClient_deliveryToken dt) {
    syslog(LOG_NOTICE, "Message with token value %d delivery confirmed", dt);
    deliveredtoken = dt;
}

int onmessage(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payloadptr = message->payload;
    char msg[message->payloadlen+1];
    strcpy(msg, payloadptr);
    msg[message->payloadlen] = '\0';

    //syslog(LOG_NOTICE, "Message arrived");
    NotifyNotification * notification = notify_notification_new (topicName,
                                                                 msg,
                                                                 "dialog-information");
    notify_notification_show (notification, NULL);
    g_object_unref(G_OBJECT(notification));

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void ondisconnect(void *context, char *cause) {
    syslog(LOG_NOTICE, "Connection lost: %s", cause);
}

int main(void) {
    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    syslog (LOG_NOTICE, "MQTT daemon started.");

    /* Daemon-specific initialization goes here */
    /* initalize libnotify */
    notify_init ("mqttcd");

    /* initalize mqtt client */
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    /* create mqtt client */
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    /* set mqtt callbacks */
    MQTTClient_setCallbacks(client, NULL, ondisconnect, onmessage, ondelivered);

    /* connect to mqtt */
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        syslog (LOG_NOTICE, "Failed to connect, return code: %d", rc);
        exit(-1);
    }

    /* subscribing to topic */
    syslog (LOG_NOTICE, "Subscribing to topic %s for client %s using QoS:%d", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);

    while (1) {
        sleep(3); // wait 3 seconds
    }

    notify_uninit();
    syslog (LOG_NOTICE, "MQTT daemon terminated.");
    exit(EXIT_SUCCESS);
}
