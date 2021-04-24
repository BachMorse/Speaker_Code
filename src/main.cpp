
#include <Arduino.h>
#include <time.h>
#include <vector>
#include "MQTT copy.cpp"
#include <string>
#include <ezButton.h>

#include "SoundData.h"
#include "XT_DAC_Audio.h"


#include "WiFi.h"
#include "PubSubClient.h" //pio lib install "knolleary/PubSubClient"

//define router and broker
#define SSID          "OnePlus jeff"///"NETGEAR68"
#define PWD           "jeffhotspot"//"excitedtuba713"

#define MQTT_SERVER   "192.168.43.143" // 192.168.1.2 // could change if the setup is moved
#define MQTT_PORT     1883



#define START     1 //button 
#define STOP      0 //button
#define SOUND_RESOLUTION    8 // 8 bit resolution
#define geluid_on            (1<<(SOUND_RESOLUTION-1)) // 50% van duty cycle
#define geluid_off         0                         // 0% van duty cycle




//wav files afspelen
XT_Wav_Class Sound(jeff); // met jeff gegenereerde wav file (hexadecimaal)
XT_DAC_Audio_Class DacAudio(25,0); //pins
uint32_t DemoCounter=0;

int Pin =25;

int freq = 2000;
int channel = 0;
int resolution = 8;

ezButton button(18); 
int status =STOP;
  
int lengte_pt= 100;
int lengte_str=10*lengte_pt;

String st="";
String s_num="";
String s_alf="";
char ch;
std::vector< String > array;
std::vector< char* > array_char;

//mqtt
bool reset=false;
bool connected;
bool gestart;
bool b=false;
bool rinkel=false;
const char* topic_speaker = "esp32/morse/speaker";
const char* topic_intern = "esp32/morse/intern";
const char* topic_control = "esp32/morse/control";
const char* topic_telefoon= "esp32/fitness/telefoon";

//String s;
//s=getStringArray();
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


//MQTT
//define callback method
void callback(char *topic, byte *message, unsigned int length);

/*void reseteu(){
   if(b=false){
    b=true;
    }
}
*/

// function for establishing wifi connection, do not touch
void setup_wifi(){
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
  Serial.print(". Message: ");
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
  if (String(topic)=="esp32/morse/intern"){

  }
  if (String(topic) == "esp32/morse/control")
  {
    if(messageTemp.equals("start")){
      gestart=true;
    }
    if(messageTemp.equals("stop")){         
      gestart=false;
      
    }
    if(messageTemp.equals("reset")){
      gestart=false;
      reset=true;
    }
  }
  if(String(topic)== topic_telefoon){
    if(messageTemp.equals("BEL")){
      rinkel=true;
    }

  }
}

// function to establish MQTT connection

void reconnect()
{
  delay(10);
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(topic_speaker))
    {
      Serial.println("connected");
      // Publish
      client.publish("esp32/morse/control", "start");
      // ... and resubscribe
      client.subscribe(topic_intern);
     Serial.print("gelukt");
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


void tone(int pin, int frequentie, int duration){
  ledcSetup(channel, frequentie, SOUND_RESOLUTION); 
  ledcAttachPin(pin, channel);                      
  /*ledcWriteTone(channel, geluid_on);
  delay(duration);
  ledcWriteTone(channel, geluid_off);  
  */
 ledcWriteTone(channel, frequentie);
 delay(duration);
 ledcWriteTone(channel,0);
}

String getKar(){
      char kar[26] = {(char)'a',(char)'b',(char)'c',(char)'d',(char)'e' ,(char)'f',(char)'g',(char)'h',(char)'i',(char)'j',(char)'k',(char)'l',(char)'m',(char)'n',(char)'o',(char)'p',(char)'q',(char)'r',(char)'s',(char)'t',(char)'u',(char)'v',(char)'w',(char)'x',(char)'y',(char)'z'}; 


String res=" " ;
 
  for (int i = 0; i < 4; i++) {
        res = res + kar[rand() % 26]; 
    }
      
    return res; 
} 

void Punt(){
  tone(Pin,1200,lengte_pt); //toon van 400Hz
// delay(600);              // delay tss twee signalen
delay(300);
//verwijzing kort signaal
  array.push_back("01");

} 
 

void Streep(){
  tone(Pin, 1200,lengte_str);  // speel toon
 // delay(600);          // delay tussen twee signalen
  delay(300);
 //verwijzing naar lang signaal
  array.push_back("012");
}

/*String getStringArray(){
  for(int i=0;i<array.size();i++){
    return array[i];
  }
}
*/
  void getA(){ 
  Punt();
  delay(100);
  Streep();
  delay(100);
}
  void getB(){
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
  void getC(){
    Streep();
    delay(100);
    Punt();
    delay(100);
    Streep();
    delay(100);
    Punt();
    delay(100);
    }
  void getD(){
    Streep();
    delay(100);
    Punt();
    delay(100);
    Punt();
    delay(100); 
   }
  void getE(){
    Punt();
    delay(100);
    }
  void getF(){
    Punt();
    delay(100);
    Punt();
    delay(100);
    Streep();
    delay(100);
    Punt();
    delay(100);
    }
  void getG(){
    Streep();
    delay(100);
    Streep();
    delay(100);
    Punt();
    delay(100);
    }
  void getH(){
    Punt();
    delay(100);
    Punt();
    delay(100);
    Punt();
    delay(100);
    Punt();
    delay(100);
    }
  void getI(){
    Punt();
    delay(100);
    Punt();
    delay(100);
    }
  void getJ(){
    Punt();
    delay(100);
    Streep();
    delay(100);
    Streep();
    delay(100);
    Streep();
    delay(100);
    }
  void getK(){
    Streep();
    delay(100);
    Punt();
    delay(100);
    Streep();
    delay(100);
  }
  void getL(){
    Punt();
    delay(100);
    Streep();
    delay(100);
    Punt();
    delay(100);
    Punt();
    delay(100);
   }
  void getM(){
    Streep();
    delay(100);
    Streep();
    delay(100);
    }
  void getN(){
    Streep();
    delay(100);
    Punt();
    delay(100);
    }
  void getO(){
    Streep();
    delay(100);
    Streep();
    delay(100);
    Streep();
    delay(100);
    }
  void getP(){
    Punt();
    delay(100);
    Streep();
    delay(100);
    Streep();
    delay(100);
    Punt();
    delay(100);
    }
  void getQ(){
    Streep();
    delay(100);
    Streep();
    delay(100);
    Punt();
    delay(100);
    Streep();
    delay(100);
    }
  void getR(){
    Punt();
    delay(100);
    Streep();
    delay(100);
    Punt();
    delay(100);
    }
  void getS(){
    Punt();
    delay(100);
    Punt();
    delay(100);
    Punt();
    delay(100);
   }
  void getT(){
    Streep();
    delay(100);
    }
  void getU(){
    Punt();
    delay(100);
    Punt();
    delay(100);
    Streep();
    delay(100);
    }
  void getV(){
    Punt();
    delay(100);
    Punt();
    delay(100);
    Punt();
    delay(100);
    Streep();
    delay(100);
    }
  void getW(){
    Punt();
    delay(100);
    Streep();
    delay(100);
    Streep();
    delay(100);
    }
  void getX(){
    Streep();
    delay(100);
    Punt();
    delay(100);
    Punt();
    delay(100);
    Streep();
    delay(100);
    }
  void getY(){
    Streep();
    delay(100);
    Punt();
    delay(100);
    Streep();
    delay(100);
    Streep();
    delay(100);
    }
  void getZ(){
    Streep();
    delay(100);
    Streep();
    delay(100);   
    Punt();
    delay(100);
    Punt();
    delay(100);
    }


void Morse(){
  if ( ch == 'a')
  {
    getA();
  //  Serial.print("a");
  }
  else if ( ch == 'b')
  {
    getB();
  //  Serial.print("b");
  }
  else if ( ch == 'c')
  {
    getC();
  //  Serial.print("c");
  }
  else if ( ch == 'd')
  {
    getD();
  //  Serial.print("d");
  }
  else if (  ch == 'e')
  {
    getE();
 //  Serial.print("e");
  }
  else if ( ch == 'f')
  {
    getF();
   // Serial.print("f");
  }
  else if ( ch == 'g')
  {
    getG();
  //  Serial.print("g");
  }
  else if ( ch == 'h')
  {
    getH();
  //  Serial.print("h");
  }
  else if (ch == 'i')
  {
   getI();
  //  Serial.print("i");
  }
  else if ( ch == 'j')
  {
    getJ();
  //  Serial.print("j");
  }
  else if ( ch == 'k')
  {
    getK();
   // Serial.print("k");
  }
  else if (ch == 'l')
  {
    getL();
   // Serial.print("l");
  }
  else if (ch == 'm')
  {
    getM();
  //  Serial.print("m");
  }
  else if ( ch == 'n')
  {
    getN();
  //  Serial.print("n");
  }
  else if ( ch == 'o')
  {
    getO();
  //  Serial.print("o");
  }
  else if ( ch == 'p')
  {
    getP();
  //  Serial.print("p");
  }
  else if ( ch == 'q')
  {
    getQ();
  //  Serial.print("q");
  }
  else if ( ch == 'r')
  {
    getR();
  //  Serial.print("r");
  }
  else if ( ch == 's')
  {
    getS();
  //  Serial.print("s");
  }
  else if ( ch == 't')
  {
   getT();
  //  Serial.print("t");
  }
  else if ( ch == 'u')
  {
    getU();
 //   Serial.print("u");
  }
  else if ( ch == 'v')
  {
    getV();
  //  Serial.print("v");
  }
  else if (
     ch == 'w')
  {
    getW();
  //  Serial.print("w");
  }
  else if ( ch == 'x')
  {
    getX();
  //  Serial.print("x");
  } 
  else if ( ch == 'y')
  {
    getY();
  //  Serial.print("y");
  }
  else if ( ch == 'z')
  {
    getZ();
  //  Serial.print("z");
  }
} 

void playRinkeltoon(){
//rinkeltoon telefoon afspelen
  DacAudio.FillBuffer();               
  if(Sound.Playing==false){
    DacAudio.Play(&Sound);      
  } 
 Serial.println(DemoCounter++); 
}
String printArrayNum(){
  //array van 012 signaaal printen
for(int i=0;i<array.size();i++){
//Serial.print(array[i]);
   s_num=array[i];
  }
  return s_num;
}

void printArrayAlf(){
//array volgens Latijns alfabet
st=getKar();
Serial.println(st);
} 

void playMorse(){

//morsecode afspelen
  int i=0;
while(i<4){
//while(reset!=true&&!correct){ // zolang er geen reset gegeven wordt

//array van 012 signaaal printen
printArrayNum();

//array volgens Latijns alfabet
printArrayAlf();

ledcWriteTone(channel,2000);// start
delay(2000);
ledcWriteTone(channel,0);
delay(2000);


//char array
/*std::transform(std::begin(array), std::end(array),
               std::back_inserter(array_char),
               [](std::string& s){ s.push_back(0); return &s[0]; });
array_char.push_back(nullptr);
char** carray = array_char.data();
*/

//omzetten naar audio
for(int i=0;i<st.length();i++){
   ch=st.charAt(i);
   Morse();
 }
 /*  int i=0;
  while(i<10){
  tone(25, 1200, 400);
  delay(400);
  }
  */
  delay(1000);
 // boolean reset=false;


ledcWriteTone(channel,100);  //stop 
delay(3000);
ledcWriteTone(channel,0);
delay(10000); // 10sec wachten tot 
}
}


void setup() {

  Serial.begin(115200);
  pinMode(18,INPUT);
 
  button.setDebounceTime(50); //debouncen

  ledcSetup(channel, freq, resolution);
  ledcAttachPin(25, channel);
  
//  client.publish("esp32/morse/intern",st);
 
//MQTT
  
    gestart = true;
    //setup wifi
    setup_wifi();
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);


   //pinMode(LED_PIN, OUTPUT);
}
  
void loop() {
//MQTT.getString();
  if(rinkel==true){
    button.loop();
  
    playRinkeltoon();

    if(button.isPressed()){
     if (status == STOP){
           status = START;
     }
      else status = STOP;
    }

    if (status == START) {
     playMorse();
    } 
   
    //MQTT  
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
   
    //PUBLISH'n naar broker
    client.publish(topic_speaker, "hey"); 
 
  }
}
/*
void printarray(){
  int n=s_num.length();
  char char_array[n + 1];
  strcpy(char_array, s_num.c_str());
 
    for (int i = 0; i < n; i++)
         char_array[i];
       client.publish(topic_speaker,);
}
*/

    /* ledcWriteTone(channel, freq);
     delay(4000);
     ledcWriteTone(channel, 440);
     }sdf qs
     */
 
