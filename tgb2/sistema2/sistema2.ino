/*
Medição de peso usando um string gauge (HX711) e envio dos dados via MQTT através de Wi-Fi. 
Configuração da conexão, inicialização do sensor de peso, conexão à Wi-Fi, inicialização do servidor e cliente MQTT.
No loop principal, leitura do peso e o publicação do valor medido na balança regularmente. 

Utilização de um ESP32.

Utilização da biblioteca TinyMqtt:
- o tinymqtt provê o broker e um client local dentro do próprio ESP
- o grafana acessa o broker e exibe o que ta passando pelo tópico
*/

#include <Arduino.h>
#include <TinyMqtt.h>
#include <Wire.h>
#include "HX711.h"
#include "soc/rtc.h"

#define CALCULATED_SCALE 450926.3333

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 23;
const int LOADCELL_SCK_PIN = 18;

HX711 scale;

const char* ssid = "";
const char* password = "";

std::string scale_topic = "esp32/scale";

MqttBroker broker(1883);
MqttClient client(&broker);

void onPublish(const MqttClient* /* source */, const Topic& topic, const char* payload, size_t /* length */) {
  Serial << "--> client received from: " << topic.c_str() << ": " << payload << endl;
}

void setup() {
  Serial.begin(115200);

  Serial.println("HX711 Demo");

  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());  // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));  // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));  // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
                                          // by the SCALE parameter (not set yet)

  scale.set_scale(CALCULATED_SCALE); // this value is obtained by calibrating the scale with known weights
  scale.tare();  // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());  // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));  // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));  // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight, divided

  Serial.println("Readings:");

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

  float weight = scale.get_units(20);

  if (millis() > timer) {
    Serial.println("");
    Serial << "Client is publishing to: " << scale_topic.c_str() << endl;
    client.publish(scale_topic, std::string(String(weight).c_str()));

    timer += publishInterval;
  }

  scale.power_down();  // put the ADC in sleep mode
  delay(5000);
  scale.power_up();
}
