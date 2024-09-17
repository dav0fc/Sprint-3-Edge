# Simulação de Velocidade de Carro de Corrida com Node-RED, Arduino Uno e MPU6050

Este projeto utiliza o Arduino Uno em conjunto com o sensor MPU6050 (acelerômetro e giroscópio) para simular a velocidade de um carro de corrida. O código também coleta mudanças de latitude, longitude, temperatura e nível de bateria, enviando essas informações em formato JSON para o Node-RED.

## Objetivo

Capturar e processar dados de aceleração do MPU6050 para simular a velocidade de um carro, além de monitorar a temperatura, posição (latitude/longitude) e o estado da bateria. O Node-RED será usado para visualizar os dados em tempo real.

## Componentes Necessários

- Arduino Uno
- Sensor MPU6050
- LED (opcional, conectado ao pino 13 para sinalização)
- Node-RED instalado em seu sistema para exibição dos dados

### Bibliotecas Arduino:

- `Wire.h`
- `MPU6050.h`
- `ArduinoJson.h`

## Esquema de Conexão

- VCC (MPU6050) → 5V no Arduino Uno
- GND (MPU6050) → GND no Arduino Uno
- SCL (MPU6050) → Pino A5 (SCL) no Arduino Uno
- SDA (MPU6050) → Pino A4 (SDA) no Arduino Uno
- LED (opcional) → Pino 13 no Arduino Uno (para sinalização de alarme de temperatura)

## Código Arduino

Este código coleta os dados de aceleração do MPU6050 e simula a velocidade do carro, além de monitorar a temperatura e o nível de bateria. Os dados são enviados ao Node-RED em formato JSON.

```cpp
#include <ArduinoJson.h>
#include <MPU6050.h>

MPU6050 mpu;  // cria um objeto MPU6050

#define LED_PIN 13     // pino do LED para indicar o alarme de temperatura
#define TAMANHO_JSON 256  // define o tamanho do buffer para o JSON

// Variáveis de velocidade, coordenadas, temperatura e bateria
float speed = 40.0;
bool aumentando = true;  // variável para controlar se estamos aumentando ou diminuindo a velocidade
float latitude = -23.5505;  // inicializado em São Paulo
float longitude = -46.6333; // inicializado em São Paulo
float temperatura = 0.0;
float bateria = 10.0;  
unsigned long previousTime = 0;
unsigned long previousBatteryTime = 0;

void setup() {
  Serial.begin(9600);  // inicializa a comunicação serial
  mpu.initialize();  // inicializa o sensor MPU6050

  // verifica se o MPU-6050 está conectado corretamente
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1);
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
    latitude += 0.001;  
    longitude += 0.001; 
    if (latitude > 90.0) latitude = -90.0;  
    if (longitude > 180.0) longitude = -180.0;  
    previousTime = currentTime;
  }

  // Atualiza o nível da bateria a cada 5 segundos
  if (currentTime - previousBatteryTime >= 5000) {
    bateria -= 2.0;  
    if (bateria < 0) bateria = 10.0;  
    previousBatteryTime = currentTime;
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

  delay(1000);  
}
cpp´´´

## Colaboradores do Projeto
<div style="display: flex; justify-content: space-between; align-items: center;">
<a href="https://github.com/miguelkapicius" target="_blank" style="text-align: center; margin-right: 10px;">
<img loading="lazy" src="https://avatars.githubusercontent.com/miguelkapicius" width=120>
<p style="font-size:min(2vh, 36px); margin-top: 10px;">Miguel Kapicius Caires - RM 556198</p>
</a>
<a href="https://github.com/dav0fc" target="_blank" style="text-align: center; margin-right: 10px;">
<img loading="lazy" src="https://avatars.githubusercontent.com/dav0fc" width=120>
<p style="font-size:min(2vh, 36px); margin-top: 10px;">David Gabriel Gomes Fernandes - RM 556020</p>
</a>
<a href="https://github.com/desenise" target="_blank" style="text-align: center; margin-right: 10px;">
<img loading="lazy" src="https://avatars.githubusercontent.com/desenise" width=120>
<p style="font-size:min(2vh, 36px); margin-top: 10px;">Denise Senise - RM 556006</p>
</a>
<a href="https://github.com/gab-gouvea" target="_blank" style="text-align: center; margin-right: 10px;">
<img loading="lazy" src="https://avatars.githubusercontent.com/gab-gouvea" width=120>
<p style="font-size:min(2vh, 36px); margin-top: 10px;">Gabriel Gouvea Marques de Oliveira - RM 555528</p>
</a>
<a href="https://github.com/Thiago-ferreirazz" target="_blank" style="text-align: center; margin-right: 10px;">
<img loading="lazy" src="https://avatars.githubusercontent.com/Thiago-ferreirazz" width=120>
<p style="font-size:min(2vh, 36px); margin-top: 10px;">Thiago Ferreira Oliveira - RM 555608</p>
</a>
</div>
