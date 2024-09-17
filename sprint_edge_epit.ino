#include <ArduinoJson.h>
#include <MPU6050.h>

MPU6050 mpu;  // cria um objeto MPU6050

#define LED_PIN 13     // pino do LED para indicar o alarme de temperatura
#define TAMANHO_JSON 256  // define o tamanho do buffer para o JSON

// Variáveis de velocidade, coordenadas, temperatura e bateria
float speed = 40.0;
bool aumentando = true;  // variável para controlar se estamos aumentando ou diminuindo a velocidade
float latitude = -23.5505;  // variável de latitude (inicializada em São Paulo, Brasil)
float longitude = -46.6333; // variável de longitude (inicializada em São Paulo, Brasil)
float temperatura = 0.0;   // variável para armazenar a temperatura
float bateria = 10.0;      // variável para armazenar o nível da bateria
unsigned long previousTime = 0;  // variável para armazenar o último tempo que a coordenada foi atualizada
unsigned long previousBatteryTime = 0; // variável para armazenar o último tempo que a bateria foi atualizada

void setup() {
  Serial.begin(9600);  // inicializa a comunicação serial
  mpu.initialize();  // inicializa o sensor MPU6050

  // verifica se o MPU-6050 está conectado corretamente
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1);  // trava o código se a conexão falhar
  }

  pinMode(LED_PIN, OUTPUT);  // configura o pino do LED como saída
}

void loop() {
  // atualiza os dados do sensor
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Obtém a temperatura em graus Celsius
  temperatura = mpu.getTemperature() / 340.00 + 36.53;

  // Converte as leituras do acelerômetro em valores de aceleração em g
  float fax = (float)ax / 16384.0;
  float fay = (float)ay / 16384.0;
  float faz = (float)az / 16384.0;

  // Atualiza a velocidade
  if (aumentando) {
    speed += 1.0;  // aumenta a velocidade em 1 km/h por segundo
    if (speed >= 100.0) {
      aumentando = false;  // muda para diminuir quando atingir 100 km/h
    }
  } else {
    speed -= 1.0;  // diminui a velocidade em 1 km/h por segundo
    if (speed <= 40.0) {
      aumentando = true;  // muda para aumentar quando atingir 40 km/h
    }
  }

  // Atualiza as coordenadas a cada 5 segundos
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= 5000) {
    latitude += 0.001;  // incrementa a latitude (simulando movimento norte/sul)
    longitude += 0.001; // incrementa a longitude (simulando movimento leste/oeste)
    if (latitude > 90.0) latitude = -90.0;  // reinicia a latitude se ultrapassar o máximo
    if (longitude > 180.0) longitude = -180.0;  // reinicia a longitude se ultrapassar o máximo
    previousTime = currentTime;  // armazena o tempo da última atualização
  }

  // Atualiza o nível da bateria a cada 5 segundos
  if (currentTime - previousBatteryTime >= 5000) {
    bateria -= 2.0;  // diminui a bateria em 2 unidades
    if (bateria < 0) bateria = 10.0;  // reinicia a bateria se chegar a 0
    previousBatteryTime = currentTime;  // armazena o tempo da última atualização da bateria
  }

  // Cria um objeto JSON para armazenar os dados
  StaticJsonDocument<TAMANHO_JSON> jsonDoc;
  
  // Limita a precisão da velocidade, aceleração, temperatura e coordenadas
  jsonDoc["velocidade_kmh"] = String(speed, 2);

  // Cria um objeto para aceleração e limita para 2 casas decimais
  JsonObject aceleracao = jsonDoc.createNestedObject("aceleracao");
  aceleracao["ax"] = String(fax, 2);
  aceleracao["ay"] = String(fay, 2);
  aceleracao["az"] = String(faz, 2);

  // Adiciona a coordenada ao JSON
  JsonObject coordenadas = jsonDoc.createNestedObject("coordenadas");
  coordenadas["latitude"] = String(latitude, 6);
  coordenadas["longitude"] = String(longitude, 6);

  // Adiciona a temperatura ao JSON
  jsonDoc["temperatura"] = String(temperatura, 2);

  // Adiciona o nível da bateria ao JSON
  jsonDoc["bateria"] = String(bateria, 1);

  // Converte o objeto JSON para uma string e envia para o monitor serial
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  Serial.println(jsonString);

  delay(1000);  // Aguarda 1 segundo antes de ler novamente
}
