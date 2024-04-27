#include <Arduino_JSON.h>
#include <PubSubClient.h>

// Definición de pines para los sensores y LEDs
const byte pinObs1 = 12;
const byte pinObs2 = 13;
const byte pinObs3 = 15;

const byte redLed1 = 16;
const byte greenLed1 = 5;

const byte redLed2 = 4;
const byte greenLed2 = 0;

const byte redLed3 = 2;
const byte greenLed3 = 14;

// Variable booleana para controlar el envío de mensajes MQTT
bool flag = false;

// Variables para almacenar el estado anterior de los sensores
int lastObstaculo1 = HIGH;
int lastObstaculo2 = HIGH;
int lastObstaculo3 = HIGH;

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Configuración de la red WiFi y MQTT
const char* ssid = "FAMILIA MORALES";
const char* password = "JKMZ2464309";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

// Configuración inicial de la conexión WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Manejador de mensajes MQTT entrantes
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// Función de reconexión al servidor MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("/grup/sensores/Morales", "si furulo we");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Configuración inicial
void setup() {
  // Configuración de pines y conexión inicial
  pinMode(redLed1, OUTPUT);
  pinMode(greenLed1, OUTPUT);
  pinMode(redLed2, OUTPUT);
  pinMode(greenLed2, OUTPUT);
  pinMode(redLed3, OUTPUT);
  pinMode(greenLed3, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// Ciclo principal
void loop() {
  // Verificación y manejo de la conexión MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Lectura de los estados actuales de los sensores
  int obstaculo1 = digitalRead(pinObs1);
  int obstaculo2 = digitalRead(pinObs2);
  int obstaculo3 = digitalRead(pinObs3);

  // Creación de un objeto JSON para almacenar los datos de los sensores
  JSONVar mensajeJSON;

  // Verificación de cambios en el sensor 1
  if (obstaculo1 != lastObstaculo1) {
    lastObstaculo1 = obstaculo1;
    flag = true;
  }

  // Procesamiento del sensor 1
  if (flag) {
    // Actualización de LEDs y publicación MQTT para el sensor 1
    if (obstaculo1 == LOW) {
      Serial.println(obstaculo1);
      digitalWrite(redLed1, HIGH);
      digitalWrite(greenLed1, LOW);
    } else {
      Serial.println("Despejado");
      digitalWrite(greenLed1, HIGH);
      digitalWrite(redLed1, LOW);
    }
    mensajeJSON["sensor10"] = obstaculo1;
    delay(150);
  }

  // Verificación de cambios en el sensor 2
  if (obstaculo2 != lastObstaculo2) {
    lastObstaculo2 = obstaculo2;
    flag = true;
  }

  // Procesamiento del sensor 2
  if (flag) {
    // Actualización de LEDs y publicación MQTT para el sensor 2
    if (obstaculo2 == LOW) {
      Serial.println("Obstaculo Detectado");
      digitalWrite(redLed2, HIGH);
      digitalWrite(greenLed2, LOW);
    } else {
      Serial.println("Despejado");
      digitalWrite(greenLed2, HIGH);
      digitalWrite(redLed2, LOW);
    }
    mensajeJSON["sensor11"] = obstaculo2;
    delay(150);
  }

  // Verificación de cambios en el sensor 3
  if (obstaculo3 != lastObstaculo3) {
    lastObstaculo3 = obstaculo3;
    flag = true;
  }

  // Procesamiento del sensor 3
  if (flag) {
    // Actualización de LEDs y publicación MQTT para el sensor 3
    if (obstaculo3 == LOW) {
      Serial.println("Obstaculo Detectado");
      digitalWrite(redLed3, HIGH);
      digitalWrite(greenLed3, LOW);
    } else {
      Serial.println("Despejado");
      digitalWrite(greenLed3, HIGH);
      digitalWrite(redLed3, LOW);
    }
    mensajeJSON["sensor12"] = obstaculo3;
    delay(150);
  }

  // Envío del mensaje MQTT si ha habido cambios
  if (flag) {
    // Conversión y envío del mensaje JSON
    String jsonStr = JSON.stringify(mensajeJSON);
    const char* mqttTopic = "/grup/sensores/Morales";
    const char* mqttPayload = jsonStr.c_str();
    Serial.println(jsonStr);
    client.publish(mqttTopic, mqttPayload);
    flag = false;
  }
}
