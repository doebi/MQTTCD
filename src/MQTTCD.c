#include "MQTTCD.h"

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
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    /* connect to mqtt */
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        syslog (LOG_NOTICE, "Failed to connect, return code %d\n", rc);
        exit(-1);
    }

    /* subscribing to topic */
    syslog (LOG_NOTICE, "Subscribing to topic %s for client %s using QoS:%d", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);

    while (1) {
        sleep(30); // wait 30 seconds
    }

    notify_uninit();
    syslog (LOG_NOTICE, "MQTT daemon terminated.");
    exit(EXIT_SUCCESS);
}
