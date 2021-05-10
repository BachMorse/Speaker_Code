
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
#define SSID  "WiFi-2.4-A4F8" //"OnePlus jeff" //"NETGEAR68"// //
#define PWD "w23bdj3rxsd4u"// "IKELPIGYQDHQWVSP" //"w23bdj3rxsd4u" //"jeffhotspot"   //"excitedtuba713"        //"NN53u7De3Swg"   // ////

#define MQTT_SERVER "broker.hivemq.com" // 192.168.1.2 // could change if the setup is moved
#define MQTT_PORT 1883

#define START 1                                 //button
#define STOP 0                                  //button
#define SOUND_RESOLUTION 8                      // 8 bit resolution
#define geluid_on (1 << (SOUND_RESOLUTION - 1)) // 50% van duty cycle
#define geluid_off 0                            // 0% van duty cycle

//wav files afspelen
XT_Wav_Class Sound(ringtone);       // met ringtone gegenereerde wav file (hexadecimaal)
XT_DAC_Audio_Class DacAudio(25, 0); //pins
uint32_t DemoCounter = 0;

int Pin = 25;

int freq = 2000;
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

char arr[44] = {'0'};
int loper = 0;
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

bool opgenomen=false;

const char *topic_speaker = "esp32/morse/speaker";
const char *topic_intern = "esp32/morse/intern";
const char *topic_control = "esp32/morse/control";
const char *topic_telefoon = "esp32/fitness/telefoon";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

/*
 char *convert(const std::string & s)
{
  
   char *pc = new char[s.size()+1];
   std::strcpy(pc, s.c_str());
   return pc; 
   
   //return s.c_str();
}

char *getArray(){
  std::transform(array.begin(), array.end(), std::back_inserter(array_char), convert);  

       for ( size_t i = 0 ; i < array_char.size() ; i++ ){

            return array_char[i];
           // client.publish(topic_speaker,array_char[i]);
           // Serial.println(array_char[i]);
       }
       for (int i = 0; i < array_char.size() && i < arr; i++) {
            arr[i] = array_char[i];
}
       for ( size_t i = 0 ; i < array_char.size() ; i++ )
            delete [] array_char[i];
            
} 
  */

//MQTT
//define callback method

// void callback(char *topic, byte *message, unsigned int length);

// function for establishing wifi connection, do not touch

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

  // Feel free to add more if statements to control more GPIOs with MQTT
  // When receiving a message on "esp32/control" a check should be excecuted

  // If a message is received on the topic esp32/control, you check if the message is either "start" or "stop" (or "reset").
  // Changes the state according to the message
  if (String(topic) == "esp32/morse/intern")
  {
    if (messageTemp.equals("correct"))
    {
      // Serial.print("correcte code");
      code_correct = true;
      gestart = false;
    }
  }

  // Main broker
  if (String(topic) == "esp32/morse/control")
  {
    if (messageTemp.equals("0")) //RESET morseproef
    {
      /*
      gestart = false;
      reset = true;
      code_correct = false;
      rinkel = false;
      */
     status=STOP;
     Serial.println("RESET");
      WiFi.disconnect();                                    
      delay(1000);
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
    if (messageTemp.equals("BEL"))
    {
      rinkel = true;
      gestart = true;
      reset = false;
      code_correct = false;
      
    }
  }
}

// function to establish MQTT connection

void reconnect()
{
 // delay(10);
  // Loop until we're reconnected
  Serial.println("Going in while loop MQTT connection");
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if  (client.connect("Morse_speaker"))
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
      delay(5000);
    }
  }
}

//END MQTT

void tone(int pin, int frequentie, int duration) // alternatief arduino methode tone()
{
  ledcSetup(channel, frequentie, SOUND_RESOLUTION);
  ledcAttachPin(pin, channel);
  /*ledcWriteTone(channel, geluid_on);
  delay(duration);
  ledcWriteTone(channel, geluid_off);  
  */
  ledcWriteTone(channel, frequentie);
  delay(duration);
  ledcWriteTone(channel, 0);
}

String getKar()  //random string van aanmaken
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
  tone(Pin, 1200, lengte_pt); //speel toon met 
  // delay(600);              // delay tss twee signalen
  /*
  client.publish(topic_intern, "0");
  client.publish(topic_intern, "1");
  */
  delay(300);

  //verwijzing kort signaal
  //array.push_back("01");
  /*
  arr[loper]={'0'};
  loper++;
  arr[loper]='1';
  loper++;
*/
}

void Streep()
{
  tone(Pin, 1200, lengte_str); // speel toon
  // delay(600);          // delay tussen twee signalen
/*
  client.publish(topic_intern, "0");
  client.publish(topic_intern, "1");
  client.publish(topic_intern, "2");
*/
  delay(300);

  //verwijzing naar lang signaal
  //array.push_back("012");
  /* 
  arr[loper]={'0'};
  loper++;
  arr[loper]='1';
  loper++;
  arr[loper]='2';
  loper++;
*/

  //client.publish(topic_speaker, "012");
}

void getA()
{
  Punt();
  delay(100);
  Streep();
  delay(100);
}
void getB()
{
  Punt();
  delay(100);
  Streep();
  delay(100);
  Punt();
  delay(100);
  Punt();
  delay(100);
  Punt();
  delay(100);
}
void getC()
{
  Streep();
  delay(100);
  Punt();
  delay(100);
  Streep();
  delay(100);
  Punt();
  delay(100);
}
void getD()
{
  Streep();
  delay(100);
  Punt();
  delay(100);
  Punt();
  delay(100);
}
void getE()
{
  Punt();
  delay(100);
}
void getF()
{
  Punt();
  delay(100);
  Punt();
  delay(100);
  Streep();
  delay(100);
  Punt();
  delay(100);
}
void getG()
{
  Streep();
  delay(100);
  Streep();
  delay(100);
  Punt();
  delay(100);
}
void getH()
{
  Punt();
  delay(100);
  Punt();
  delay(100);
  Punt();
  delay(100);
  Punt();
  delay(100);
}
void getI()
{
  Punt();
  delay(100);
  Punt();
  delay(100);
}
void getJ()
{
  Punt();
  delay(100);
  Streep();
  delay(100);
  Streep();
  delay(100);
  Streep();
  delay(100);
}
void getK()
{
  Streep();
  delay(100);
  Punt();
  delay(100);
  Streep();
  delay(100);
}
void getL()
{
  Punt();
  delay(100);
  Streep();
  delay(100);
  Punt();
  delay(100);
  Punt();
  delay(100);
}
void getM()
{
  Streep();
  delay(100);
  Streep();
  delay(100);
}
void getN()
{
  Streep();
  delay(100);
  Punt();
  delay(100);
}
void getO()
{
  Streep();
  delay(100);
  Streep();
  delay(100);
  Streep();
  delay(100);
}
void getP()
{
  Punt();
  delay(100);
  Streep();
  delay(100);
  Streep();
  delay(100);
  Punt();
  delay(100);
}
void getQ()
{
  Streep();
  delay(100);
  Streep();
  delay(100);
  Punt();
  delay(100);
  Streep();
  delay(100);
}
void getR()
{
  Punt();
  delay(100);
  Streep();
  delay(100);
  Punt();
  delay(100);
}
void getS()
{
  Punt();
  delay(100);
  Punt();
  delay(100);
  Punt();
  delay(100);
}
void getT()
{
  Streep();
  delay(100);
}
void getU()
{
  Punt();
  delay(100);
  Punt();
  delay(100);
  Streep();
  delay(100);
}
void getV()
{
  Punt();
  delay(100);
  Punt();
  delay(100);
  Punt();
  delay(100);
  Streep();
  delay(100);
}
void getW()
{
  Punt();
  delay(100);
  Streep();
  delay(100);
  Streep();
  delay(100);
}
void getX()
{
  Streep();
  delay(100);
  Punt();
  delay(100);
  Punt();
  delay(100);
  Streep();
  delay(100);
}
void getY()
{
  Streep();
  delay(100);
  Punt();
  delay(100);
  Streep();
  delay(100);
  Streep();
  delay(100);
}
void getZ()
{
  Streep();
  delay(100);
  Streep();
  delay(100);
  Punt();
  delay(100);
  Punt();
  delay(100);
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

void playRinkeltoon()
{
  //rinkeltoon telefoon afspelen
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

void playMorse()
{

  //morsecode afspelen
  Serial.println("playmorse");
  
  if (test == 0)  //anders telkens nieuwe sequentie bij elke loop()
  {
    printArrayAlf();
    test = 1;
 }
  ledcWriteTone(channel, 100); // starttoon
  delay(2000);
  ledcWriteTone(channel, 0);
  delay(2000);

  //omzetten naar audio
  for (int i = 0; i < st.length(); i++)
  {
    ch = st.charAt(i);
    Morse();
  }
  

  delay(1000);

  ledcWriteTone(channel, 100); //einde toon
  delay(3000);
  ledcWriteTone(channel, 0);
  delay(20000); 

}

void setup()
{

  Serial.begin(115200);
  // pinMode(18, INPUT);

  button.setDebounceTime(50); //debouncen

  ledcSetup(channel, freq, resolution);
  ledcAttachPin(25, channel);

  //MQTT
  //gestart = true;
  //setup wifi
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  //pinMode(LED_PIN, OUTPUT);
}

void loop()
{
button.loop();
  //MQTT
  Serial.print("voor if-lus ivm client.connected()  ");////////////////////Sarah
  Serial.println(client.connected());////////////////////Sarah
  if (!client.connected())
  {
    reconnect();
    Serial.print("reconnecten");
  }
  Serial.print("voor client.loop() = na iflus  ");////////////////////Sarah
  Serial.println(client.connected());////////////////////Sarah
  client.loop();
  Serial.print("na client.loop()  ");////////////////////Sarah
  Serial.println(client.connected());////////////////////Sarah
/*
  long now = millis();
  if (now - lastMsg > 5000)
  {
    lastMsg = now;
  }
*/
  if (gestart == true && code_correct == false )
  {  
    if (status == STOP)
    {
      playRinkeltoon();

      if (i < 1)
      {
        client.publish(topic_telefoon, "ik ben aan het rinkelen...rinkel rinkel rinkel!");
        i++;
      }
    }

//Serial.println(button.getState()); 
   
   if (button.isPressed())
    {
      
     // button.isReleased();
    // client.publish(topic_telefoon, "telefoon opgenomen (knop ingedrukt)");
     
     //Serial.println(button.getState());  

      if (status == STOP)
      {
        status = START;
        //rinkel = false; 
    
      }

    /*
      else
        status = STOP;
        
*/
    }
    if(status == START)
    {
      playMorse();
      //Serial.print("na play morse  ");////////////////////Sarah
      //Serial.println(client.connected()); //////////////////Sarah
    }
  }

}
