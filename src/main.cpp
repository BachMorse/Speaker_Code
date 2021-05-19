
#include <Arduino.h>
#include <time.h>
#include <vector>
#include <string>
#include <cstring>
#include <ezButton.h>

#include "SoundData.h"
#include "XT_DAC_Audio.h"

#include "WiFi.h"
#include "PubSubClient.h" //pio lib install "knolleary/PubSubClient"

//define router and broker
#define SSID  "NETGEAR68" //"OnePlus jeff"
#define PWD    "excitedtuba713"   //""

#define MQTT_SERVER "192.168.1.2" //"broker.hivemq.com"
#define MQTT_PORT 1883

#define START 1                                 //button
#define STOP 0                                  //button
#define geluid_on (1 << (SOUND_RESOLUTION - 1)) // 50% van duty cycle
#define geluid_off 0                            // 0% van duty cycle

//wav files afspelen
XT_Wav_Class Sound(ringtone);       // met ringtone gegenereerde wav file (hexadecimaal)
XT_DAC_Audio_Class DacAudio(25, 0); //pins
uint32_t DemoCounter = 0;
int Pin = 26;

//int freq = 2000;
int channel = 0;
int resolution = 8;

ezButton button(18);
int status = STOP;
// int rinkel = 0;

int lengte_pt = 100;
int lengte_str = 10 * lengte_pt;

String st = "";
String s_num = "";
String s_alf = "";
char ch;
int test = 0;
std::vector<std::string> array;
std::vector<char *> array_char;

int i = 0;

//mqtt
bool reset = false;
bool connected;
bool gestart = false;
bool b = false;
bool rinkel = false;
bool code_correct = false;
bool pauze_afstand = false;
bool pauze_fitness = false;
bool eenmaal_voltooid = false;
bool opgenomen = false;

const char *topic_intern = "esp32/morse/intern";
const char *topic_control = "esp32/morse/control";
const char *topic_telefoon = "esp32/fitness/telefoon";
const char *topic_speaker = "esp32/morse/speaker_end";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//!!!myDelay, eigen delay aanmaken want delay() is blocking !!
unsigned long myPrevMillis;

void myDelay(int del)
{
  myPrevMillis = millis();
  unsigned long myCurrentMillis = myPrevMillis;
  while (millis() - myPrevMillis <= del)
  {
    myCurrentMillis = millis();
    client.loop();
  }
}

//MQTT
//define callback method

void callback(char *topic, byte *message, unsigned int length);

// function for establishing wifi connection, do not touch

void setup_wifi()
{
  myDelay(10);
  Serial.println("Connecting to WiFi..");

  WiFi.begin(SSID, PWD);

  while (WiFi.status() != WL_CONNECTED)
  {
    myDelay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// callback function, only used when receiving messages
void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.println(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

   if (String(topic) == "esp32/morse/intern")
  {
    if (messageTemp.equals("correct")) //spelers floten juiste code na--> speaker stopt met spelen van morse code/ esp reset
    {

      code_correct = true;
      gestart = false;
      WiFi.disconnect();
      myDelay(5000);
      ESP.restart();
     
    }
  }

  // Main broker
  if (String(topic) == "esp32/morse/control")
  {
    if (messageTemp.equals("0")) //RESET is gegeven
    {
        reset=true;
      //Serial.println("RESET");
     // WiFi.disconnect();
      
       //ESP.restart();
      
      WiFi.disconnect();
      myDelay(5000);
      ESP.restart();
    }
    if (messageTemp.equals("1")) //STOP morseproef, want niet genoeg afstand
    {

      pauze_afstand = true;
    }
    if (messageTemp.equals("2")) //START morseproef, want ze zijn ontsmet
    {
      gestart = true;
      pauze_afstand = false;
      reset = false;
      code_correct = false;
    }
    if (messageTemp.equals("3")) //STOP morseproef, want batterij leeg
    {

      pauze_fitness = true;
    }
    if (messageTemp.equals("4")) //START morseproef, want batterij terug geladen
    {
      pauze_fitness = false;
    }
  }
  if (String(topic) == topic_telefoon)
  {
    if (messageTemp.equals("BEL")) // indien fitnesstracker voltooid is wordt bericht 'BEL' verstuurd naar speaker en kan morse puzzel van start gaan
    {
      rinkel = true;
      gestart = true;
      reset = false;
      code_correct = false;
    }
  }
}

//  MQTT connection opzetten

void reconnect()
{
  myDelay(10);
  // Loop until we're reconnected
  Serial.println("Going in while loop MQTT connection");
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Morse_speaker"))
    {

      Serial.println("connected");
      // Publish
      client.publish(topic_control, "speaker wacht op BEL");
      // ... and resubscribe
      client.subscribe(topic_control);
      client.subscribe(topic_intern);
      client.subscribe(topic_telefoon);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      myDelay(5000);
    }
  }
}

//END MQTT

void tone(int pin, int frequentie, int duration) // alternatief arduino methode tone()
{
  ledcSetup(channel, frequentie, resolution);
  ledcAttachPin(pin, channel);
  /*ledcWriteTone(channel, geluid_on);
  myDelay(duration);
  ledcWriteTone(channel, geluid_off);  
  */
  ledcWriteTone(channel, frequentie);
  myDelay(duration);
  ledcWriteTone(channel, 0);
}

String getKar() //random string van aanmaken
{
  char kar[26] = {(char)'a', (char)'b', (char)'c', (char)'d', (char)'e', (char)'f', (char)'g', (char)'h', (char)'i', (char)'j', (char)'k', (char)'l', (char)'m', (char)'n', (char)'o', (char)'p', (char)'q', (char)'r', (char)'s', (char)'t', (char)'u', (char)'v', (char)'w', (char)'x', (char)'y', (char)'z'};

  String res = " ";

  for (int i = 0; i < 4; i++)
  {

    res = res + kar[esp_random() % 26];
  }

  return res;
}

void Punt()
{

  tone(Pin, 500, lengte_pt); //speel toon met freq van 500Hz

  if (eenmaal_voltooid == false)
  {
    client.publish(topic_intern, "0"); // 0,1 wordt als kort gezien (punt) ,publishen naar micro
    client.publish(topic_intern, "1");
  }
  myDelay(450); //tijd tussen twee signalen in
}

void Streep()
{

  tone(Pin, 500, lengte_str);

  if (eenmaal_voltooid == false)
  { //0,1,2 als lang signaal (streep)
    client.publish(topic_intern, "0");
    client.publish(topic_intern, "1");
    client.publish(topic_intern, "2");
  }
  myDelay(450);
}

// Elke letter omzetten naar morse
void getA()
{
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
}
void getB()
{
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
}
void getC()
{
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
}
void getD()
{
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
}
void getE()
{
  Punt();
  myDelay(100);
}
void getF()
{
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
}
void getG()
{
  Streep();
  myDelay(100);
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
}
void getH()
{
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
}
void getI()
{
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
}
void getJ()
{
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
  Streep();
  myDelay(100);
  Streep();
  myDelay(100);
}
void getK()
{
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
}
void getL()
{
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
}
void getM()
{
  Streep();
  myDelay(100);
  Streep();
  myDelay(100);
}
void getN()
{
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
}
void getO()
{
  Streep();
  myDelay(100);
  Streep();
  myDelay(100);
  Streep();
  myDelay(100);
}
void getP()
{
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
}
void getQ()
{
  Streep();
  myDelay(100);
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
}
void getR()
{
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
}
void getS()
{
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
}
void getT()
{
  Streep();
  myDelay(100);
}
void getU()
{
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
}
void getV()
{
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
}
void getW()
{
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
  Streep();
  myDelay(100);
}
void getX()
{
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
}
void getY()
{
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
  Streep();
  myDelay(100);
  Streep();
  myDelay(100);
}
void getZ()
{
  Streep();
  myDelay(100);
  Streep();
  myDelay(100);
  Punt();
  myDelay(100);
  Punt();
  myDelay(100);
}

void Morse()
{
  if (ch == 'a')
  {
    getA();
  }
  else if (ch == 'b')
  {
    getB();
  }
  else if (ch == 'c')
  {
    getC();
  }
  else if (ch == 'd')
  {
    getD();
  }
  else if (ch == 'e')
  {
    getE();
  }
  else if (ch == 'f')
  {
    getF();
  }
  else if (ch == 'g')
  {
    getG();
  }
  else if (ch == 'h')
  {
    getH();
  }
  else if (ch == 'i')
  {
    getI();
  }
  else if (ch == 'j')
  {
    getJ();
  }
  else if (ch == 'k')
  {
    getK();
  }
  else if (ch == 'l')
  {
    getL();
  }
  else if (ch == 'm')
  {
    getM();
  }
  else if (ch == 'n')
  {
    getN();
  }
  else if (ch == 'o')
  {
    getO();
  }
  else if (ch == 'p')
  {
    getP();
  }
  else if (ch == 'q')
  {
    getQ();
  }
  else if (ch == 'r')
  {
    getR();
  }
  else if (ch == 's')
  {
    getS();
  }
  else if (ch == 't')
  {
    getT();
  }
  else if (ch == 'u')
  {
    getU();
  }
  else if (ch == 'v')
  {
    getV();
  }
  else if (
      ch == 'w')
  {
    getW();
  }
  else if (ch == 'x')
  {
    getX();
  }
  else if (ch == 'y')
  {
    getY();
  }
  else if (ch == 'z')
  {
    getZ();
  }
}

void playRinkeltoon() //rinkeltoon telefoon afspelen
{

  DacAudio.FillBuffer();
  if (Sound.Playing == false)
  {
    DacAudio.Play(&Sound);
  }
  (DemoCounter++);
}

void printArrayAlf()
{
  //array volgens Latijns alfabet
  st = getKar();
  Serial.println(st);
}

void playMorse() //morsecode afspelen
{

  if (test == 0) //anders telkens nieuwe morse sequentie bij elke loop() //of in getKar zelf aanpassen
  {
    printArrayAlf();
    test = 1;
  }
  ledcWriteTone(channel, 100); // starttoon
  myDelay(2000);
  ledcWriteTone(channel, 0);
  myDelay(2000);

  //omzetten naar audio
  for (int i = 0; i < st.length(); i++)
  {
    ch = st.charAt(i);
    Morse();
  }
  eenmaal_voltooid = true;
  myDelay(1000);

  ledcWriteTone(channel, 50);             //einde toon
  client.publish(topic_speaker, "einde"); //einde aangeven aan micro
  myDelay(3000);
  ledcWriteTone(channel, 0);
  myDelay(10000);
}

void setup()
{
  Serial.begin(115200);
  //pinMode(18, INPUT);
  pinMode(16, OUTPUT);
  button.setDebounceTime(50); //debouncen van knop

  //ledcSetup(channel, freq, resolution);
  ledcAttachPin(Pin, channel); 


  //MQTT
  //setup wifi
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
}

void loop()
{

  button.loop();
  //MQTT connectie  

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

  if (gestart == true && code_correct == false && pauze_fitness == false && pauze_afstand == false && rinkel == true &&reset==false)
  {
    if (status == STOP) //status van knop
    {
      playRinkeltoon();
    }

    if (button.isPressed())
    {

      if (status == STOP)
      {
        status = START;
      }

      else
        status = STOP;
    }
    if (status == START)
    {
      playMorse();
    }
  }
}
