/*
#include "WiFi.h"
#include "PubSubClient.h" //pio lib install "knolleary/PubSubClient"

#define SSID          "Techtile"
#define PWD           "Techtile229"

#define MQTT_SERVER   "10.128.48.25"
#define MQTT_PORT     1883

#define LED_PIN       2

String str="";

const char* topic_speaker = "esp32/morse/speaker";
const char* topic_intern = "esp32/morse/intern";
const char* topic_control = "esp32/morse/control";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
boolean b=false;
void getString(){
    
}

void callback(char *topic, byte *message, unsigned int length);

 
void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to WiFi..");

  WiFi.begin(SSID, PWD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{

  Serial.begin(115200);

  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  pi
  nMode(LED_PIN, OUTPUT);
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == topic_control)
  {
    Serial.print("Changing output to ");
    if (messageTemp == "1")
    {
      Serial.println("1");
      digitalWrite(LED_PIN, HIGH);
      
       }
    else if (messageTemp == "0") //bv reset
    {
      Serial.println("0");
      digitalWrite(LED_PIN, LOW);
      reset();

    }
  }
  if(String(topic)=="esp32/morse/intern"){
  }
  if (String(topic)=="esp32/morse/speaker"){
    
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("esp32/morse/speaker"))
    {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/morse/control");
      client.subscribe("esp32/morse/intern");
      client.publish("esp32/morse/speaker", "De string van karakters");

    }

    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000)
  {
    lastMsg = now;
  }

  // PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
  if (client.publish(topic_speaker, str) {
    Serial.println("String verstuurd");
  }
 
  else {
    Serial.println("String versturen failed. Nieuwe connectie wordt aangemaakt");
   // client.connect();
    reconnect();
   // delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(topic_speaker,str );
}


*/