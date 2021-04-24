/*
#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include "WiFi.h"
#include "PubSubClient.h"


//#define SSID "NETGEAR68"
//#define PWD "excitedtuba713"
//#define MQTT_SERVER "192.168.1.2"
//#define MQTT_PORT 1883

static WiFiClient espClient;           //wifi verbing object
static PubSubClient client(espClient); //mqtt handler object

class MQTT
{

private:
    long lastMsg = 0;
    char msg[50];
    int value = 0;

public:
    MQTT();
    String lastSignal;    //laatste signaal dat binnen gekomen is
    String currentSignal; //signaal dat momenteel binnen gekomen is
    void setup();         //algemen setup van de klasse
    void setup_wifi();     //wifi verbinding setup
    void reconnect();
    void callback(char *topic, byte *message, unsigned int length); //deze methode wordt opgeroepen als er een bericht binne komt via mqtt
   void reseteu();
   void printarray(String s);
    void loop();  //ik weet niet zo goed wat dit doet
};

#endif
*/
