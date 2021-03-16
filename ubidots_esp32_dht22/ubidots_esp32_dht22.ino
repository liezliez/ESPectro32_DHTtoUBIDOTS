#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>


// Ubidots Setting

#define TOKEN "BBFF-Rcj28gT1PEl8" // Enter your Ubidots Token Device Here
#define MQTT_CLIENT_NAME "cobainaja" // Enter your client name, UTF8 String
#define VARIABLE_LABEL "kelembaban" // Your Variable, in this case im using DHT11 so its Moisture
#define VARIABLE_LABEL2 "temperatur" // Your Second Variable, this is temperature
#define DEVICE_LABEL "esp32" // Your Board/Device Name at Ubidots
#define DHTPIN 5          // DHT11 output pin
#define DHTTYPE DHT11     // DHT sensor type library

// WIFI Connection Setting

#define WIFISSID "Example123"  // Enter your SSID here
#define PASSWORD "LuwakWhiteCoffee"  //Enter your Password here

DHT dht(DHTPIN, DHTTYPE);

// Initiate and Declaration of Data

char mqttBroker[] = "things.ubidots.com";
char payload[100];
char topic[150];
char topic2[150];
char str_humidity[10];
char str_temperature[10];
float hum;
float temp;


WiFiClient ubidots;
PubSubClient client(ubidots);

void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);
  Serial.write(payload, length);
  Serial.println(topic);
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}
void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);
  dht.begin();
  Serial.println();
  Serial.print("Wait for WiFI...");

  while(WiFi.status() !=WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  dtostrf(temp, 4, 2, str_temperature);
  dtostrf(hum, 4, 2, str_humidity);
  
   //topic 1 humidity
  sprintf(topic, "%s", ""); 
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(payload,"%s","");
  sprintf(payload,"{\"%s\":",VARIABLE_LABEL);
  sprintf(payload, "%s {\"value\": %s", payload, str_humidity); 
  sprintf(payload, "%s } }", payload); 
  client.publish(topic, payload);

  //topic 2 temperature
  sprintf(topic2, "%s", ""); 
  sprintf(topic2, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(payload,"%s","");
  sprintf(payload,"{\"%s\":",VARIABLE_LABEL2);
  sprintf(payload, "%s {\"value\": %s", payload, str_temperature); 
  sprintf(payload, "%s } }", payload); 
  client.publish(topic2, payload);

  client.loop();
  delay(5000);
}
