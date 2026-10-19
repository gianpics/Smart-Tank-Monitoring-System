#ifndef __CONFIG__
#define __CONFIG__


// PIN
#define LED_G_PIN   25   // green
#define LED_2_PIN   26   // red
#define TRIG_PIN    27
#define ECHO_PIN    14

// Sonar Proximity
#define QUERY_PERIOD 1000    // ms
#define SONAR_MAX_TIME 10000 // μs


// MQTT
#define MSG_BUFFER_SIZE  50

/* WiFi network info */

#define SSID_WIFI ""
#define PASSWORD_WIFI ""

/* MQTT server address */
#define MQTT_SERVER "broker.mqtt-dashboard.com"
#define MQTT_PORT 1883
#define MQTT_CONN_ATTEMPT_COOLDOWN  3000


/* MQTT topic */
#define TOPIC "TMS-esiot-2025-Water-Level"


#endif
