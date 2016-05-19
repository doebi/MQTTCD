# MQTTCD
A MQTT Client Daemon designed to run on your notebook and notify about Messages.

# Design Decisions
* Have an internal MQTT Broker secured via SSL for communication among my personal devices.
* All messages show up on my desktop.
* Error Messages are set as retained, so they can't be lost and show up until solved and manually removed from MQTT.
* Have system maintain scripts broadcast status messages via MQTT in addition to writing logs.
