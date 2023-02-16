//****************************************//
//* ESP32 MQTT Motion Sensor Template   *//
//* Based on Rui Santos'                 *//
//* https://randomnerdtutorials.com      *//
//*                                      *//
//* ESP32 Code                           *//
//*                                      *//
//* UoN 2022 - ND                        *//
//* Altered using OpenAI. 
//* OpenAI, "Motion Sensor Code.". [Online]. Available: https://chat.openai.com/chat/97ee314a-8d03-4c0f-89e2-e333c8dd7c66. [Accessed: 15-Oct-2023].

//****************************************//

#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "REPLACE_WITH_YOUR_SSID";                      //CHANGE ME
const char* password = "REPLACE_WITH_YOUR_PASSWORD";              //CHANGE ME     

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";                      
const char* mqtt_server = "YOUR_MQTT_BROKER_IP_ADDRESS";          //CHANGE ME

// Initialize the sensor
SparkFun_APDS9960 apds;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  if (apds.init()) {
    Serial.println("APDS-9960 initialization complete");
  } else {
    Serial.println("Something went wrong during APDS-9960 init!");
  }

  // Start running the gesture detection engine
  if (apds.enableGestureSensor(true)) {
    Serial.println("Gesture sensor is now running");
  } else {
    Serial.println("Something went wrong during gesture sensor init!");
  }
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      
      // Subscribe to a topic that can be used to control the ESP32
      client.subscribe("esp32/gesture");
         
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Check if a gesture has been detected
  if (apds.isGestureAvailable()) {
    int gesture = apds.readGesture();
    Serial.print("Gesture detected: ");
    switch (gesture) {
      case DIR_UP:
        Serial.println("UP");
        client.publish("esp32/gesture", "UP");
        break;
      case DIR_DOWN:
        Serial.println("DOWN");
        client.publish("esp32/gesture", "DOWN");
        break;
      case DIR_LEFT:
        Serial.println("LEFT");
        client.publish("esp32/gesture", "LEFT");
        break;
      case DIR_RIGHT:
        Serial.println("RIGHT");
        client.publish("esp32/gesture", "RIGHT");
        break;
      default:
        Serial.println("NONE");
        client.publish("esp32/gesture", "NONE");
      
    }
  }
}

