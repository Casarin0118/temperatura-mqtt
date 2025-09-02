#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"

// Definições da sua rede Wi-Fi
const char* ssid = "WIFI-IOT";
const char* password = "Ac5ce1ss0@IOT";

// Definições do sensor DHT
#define DHTPIN 21
#define DHTTYPE DHT11

// Inicializa o sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Cria um objeto de servidor web na porta 80
WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(100);

  // Inicia a comunicação com o sensor
  dht.begin();

  // Conecta ao Wi-Fi
  Serial.print("Conectando-se a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Configura a rota da API (endpoint) para GET
  server.on("/dados", handleData);
  
  // Inicia o servidor
  server.begin();
  Serial.println("Servidor web iniciado.");
}

void loop() {
  // Mantém o servidor rodando e ouvindo por requisições GET
  server.handleClient();
}

void handleData() {
  // Adiciona o cabeçalho CORS para permitir requisições de qualquer origem
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  // Lê a umidade e a temperatura do sensor
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  // Verifica se a leitura foi bem-sucedida
  if (isnan(umidade) || isnan(temperatura)) {
    server.send(500, "text/plain", "Falha na leitura do sensor DHT!");
    return;
  }

  // Cria uma string JSON com os dados
  String jsonResponse = "{";
  jsonResponse += "\"temperatura\":" + String(temperatura, 2);
  jsonResponse += ", \"umidade\":" + String(umidade, 2);
  jsonResponse += "}";

  // Envia a resposta como JSON
  server.send(200, "application/json", jsonResponse);
}