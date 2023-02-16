//****************************************//
//* ESP32 MQTT MPU-6050 Sensor Template   *//
//* Based on Rui Santos'                 *//
//* https://randomnerdtutorials.com      *//
//*                                      *//
//* ESP32 Code                           *//
//*                                      *//
//* UoN 2022 - ND                        *//
//* Altered using OpenAI. 
//* OpenAI, "ESP32 MQTT Template.". [Online]. Available: https://chat.openai.com/chat/d4c0db5a-e817-40fd-8d51-8177d46475f0. [Accessed: 15-Oct-2023].

//****************************************//

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Replace the next variables with your SSID/Password combination
const char* ssid = "REPLACE_WITH_YOUR_SSID";                      //CHANGE ME
const char* password = "REPLACE_WITH_YOUR_PASSWORD";              //CHANGE ME     

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";                      
const char* mqtt_server = "YOUR_MQTT_BROKER_IP_ADDRESS";          //CHANGE ME

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Wire.begin(21, 22);
  mpu.initialize();
  while (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    delay(1000);
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

  // Add your subscribed topics here i.e. statements to control GPIOs with MQTT
  // --
  
  // --
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      
      // Add your subscribe topics here
      // --
      
      // --
         
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

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
  

  
    // Get accelerometer and gyroscope data
    MPU6050_data_t accelgyro = MPU6050_readAll();
    
    // Calculate roll, pitch, and yaw angles
    float roll = atan2(accelgyro.accelY, accelgyro.accelZ) * RAD_TO_DEG;
    float pitch = atan2(-accelgyro.accelX, sqrt(pow(accelgyro.accelY, 2) + pow(accelgyro.accelZ, 2))) * RAD_TO_DEG;
    float yaw = (atan2(accelgyro.gyroY, accelgyro.gyroX) * RAD_TO_DEG) + OFFSET_YAW;
    
    // Apply complementary filter
    float alpha = 0.96; // Weight of accelerometer data (0 to 1)
    roll = alpha * roll + (1 - alpha) * last_roll;
    pitch = alpha * pitch + (1 - alpha) * last_pitch;
    yaw = alpha * yaw + (1 - alpha) * last_yaw;

    // Save last roll, pitch, and yaw values for next iteration
    last_roll = roll;
    last_pitch = pitch;
    last_yaw = yaw;

    // Publish roll, pitch, and yaw values to MQTT broker
    char msg[50];
    snprintf(msg, 50, "%f", roll);
    client.publish("sensor/roll", msg);
    snprintf(msg, 50, "%f", pitch);
    client.publish("sensor/pitch", msg);
    snprintf(msg, 50, "%f", yaw);
    client.publish("sensor/yaw", msg);

    // Wait for a short time before reading data again
    delay(10);
  }
}
