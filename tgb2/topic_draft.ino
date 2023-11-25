#include <TinyMqtt.h>
#include <Wire.h>
#include "HX711.h"

//PINOS HX711
#define DT 16
#define SCK 4

const char* ssid = "cabir";
const char* password = "v9yrxx37y5kpx6s";

std::string scale_topic = "esp32/scale";

MqttBroker broker(1883);
MqttClient client(&broker);

void onPublish(const MqttClient* /* source */, const Topic& topic, const char* payload, size_t /* length */) {
  Serial << "--> client received from: " << topic.c_str() << ": " << payload << endl;
}

#define SCALE 450926.3333

HX711 escala;

void setup() {
  Serial.begin(115200);

  escala.begin(DT, SCK);

  Serial.print("Leitura do Valor ADC: ");
  Serial.println(escala.read());
  Serial.println("Nao coloque nada na balanca!");
  Serial.println("Iniciando...");

  escala.set_scale(SCALE);
  escala.tare(20);

  delay(500);
  Serial << "Connecting to Wifi " << endl;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial << '-';
    delay(500);
  }

  Serial << "Connected to " << ssid << " IP address: " << WiFi.localIP() << endl;

  broker.begin();

  client.setCallback(onPublish);
  client.subscribe(scale_topic);
}

void loop() {
  broker.loop();
  client.loop();

  static const int publishInterval = 500;  // ms
  static uint32_t timer = millis() + publishInterval;

  float weight = escala.get_units(20);

  Serial.print("\nPeso medido: ");
  Serial.println(weight);

  delay(1000);

   if (millis() > timer) {
     Serial.println("");
     Serial << "Client is publishing to: " << scale_topic.c_str() << endl;
     client.publish(scale_topic,  std::string(String(a.acceleration.x).c_str()));

     timer += publishInterval;
   }
}