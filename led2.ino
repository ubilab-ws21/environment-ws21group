// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <PubSubClient.h>


const char *ssid = "....."; // Enter your WiFi name
const char *password = ".....";  // Enter WiFi password
#define MQTT_SERVER_IP "192.168.1.113"

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
   
// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        14 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 100 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

/***********************************************************************************************************************/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("shriyaTopic", "hello sss");
      // ... and resubscribe
      //client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/******************************************************************************************************/
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    pixels.clear(); // Set all pixel colors to 'off'

    // The first NeoPixel in a strand is #0, second is 1, all the way up
    // to the count of pixels minus one.
    for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      // Here we're using a moderately bright green color:
      pixels.setPixelColor(i, pixels.Color(150, 150, 150));
      pixels.show();   // Send the updated pixel colors to the hardware.
    }
  } 
  else {
    Serial.println("Led off");
    pixels.clear();
    pixels.show();
  }
}


/***************************************************************************************************/
void setup() {
   pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

   // Set software serial baud to 115200;
   Serial.begin(115200);
  
   // connecting to a WiFi network
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
       delay(500);
       Serial.println("Connecting to WiFi..");
   }
   Serial.println("Connected to the WiFi network");
   Serial.print("IP address:");
   Serial.println(WiFi.localIP());



   //mqtt calback
   client.setServer(MQTT_SERVER_IP, 1883);
   client.setCallback(callback);
}

/**************************************************************************************/
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();

  client.publish("Env/led", "1");
  client.subscribe("Env/led");
  
}
