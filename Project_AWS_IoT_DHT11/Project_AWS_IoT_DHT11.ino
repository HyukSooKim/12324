#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h> 
#include <ArduinoJson.h>
#include "arduino_secrets.h"
#include "DHT.h"
#include "Led.h"
#include "DC.h"

#define DHTTYPE DHT11  

#define DHTPIN 11    
#define LED_1_PIN 6
#define LED_2_PIN 7
#define LED_3_PIN 8
#define LED_4_PIN 9
#define LED_5_PIN 10
#define DC_IN_A 2
#define DC_IN_B 4
#define DC_EN_A 5

DHT dht(DHTPIN, DHTTYPE);
Led led1(LED_1_PIN);
Led led2(LED_2_PIN);
Led led3(LED_3_PIN);
Led led4(LED_4_PIN);
Led led5(LED_5_PIN);
DC DC1(DC_IN_A, DC_IN_B, DC_EN_A);

const char ssid[]        = SECRET_SSID;
const char pass[]        = SECRET_PASS;
const char broker[]      = SECRET_BROKER;
const char* certificate  = SECRET_CERTIFICATE;

WiFiClient    wifiClient;            // TCP 소켓 연결에 사용 
BearSSLClient sslClient(wifiClient); // SSL/TLS 연결에 사용되며 ECC508과 통합
MqttClient    mqttClient(sslClient);

unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("1 : 자동모드, 2 : 수동모드");

  dht.begin();

  if (!ECCX08.begin()) {
    Serial.println("No ECCX08 present!");
    while (1);
  }

  // 현재 시간을 가져오도록 콜백 설정, 서버 인증서 유효성 검사에 사용
  ArduinoBearSSL.onGetTime(getTime);

  // private key에 사용할 ECCX08 슬롯 설정 + 이를 위한 인증서 첨부하는 과정
  sslClient.setEccSlot(0, certificate);

  //메시지 콜백을 설정하고 이 함수는 MQTTclient가 메시지를 받았을 때 호출된다.
  mqttClient.onMessage(onMessageReceived);
}

void loop() {
//  float temp = dht.readTemperature();
//  if(temp > 26){
//     led1.off();
//     led2.off();
//     led3.off();
//     led4.off();
//     led5.off();
//     DC1.on();
//   }
//   else if(temp > 25){
//     led1.off();
//     led2.off();
//     led3.off();
//     led4.off();
//     led5.off();
//     DC1.off();
//    }
//   else{
//     led1.on();
//     led2.on();
//     led3.on();
//     led4.on();
//     led5.on();
//     DC1.off();
//   }
//   char read_data;

  // if (Serial.available())
  //   {
  //     read_data = Serial.read();
  //     if( read_data == '1')
  //     {
  //       Serial.println("수동모드로 전환합니다");
  //     }

  //   }

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    connectMQTT();
  }

  // 새로운 mqtt 메시지를 폴링하고 계속해서 유지하기위해 보내는 역할
  mqttClient.poll();

  // 5초마다 메시지를 게시하는 과정
  // aws로 게시하는 주제는 ($aws/things/Project_MyMKRWiFi1010/shadow/update)

  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    char payload[512];
    getDeviceStatus(payload);
    sendMessage(payload);
  }

}
// 와이파이 모듈로부터 현재 시간 얻어옴
unsigned long getTime() {
  return WiFi.getTime();
}

void connectWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.print(ssid);
  Serial.print(" ");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }
  Serial.println();
  Serial.println("You're connected to the network");
  Serial.println();
}

void connectMQTT() {
  Serial.print("Attempting to MQTT broker: ");
  Serial.print(broker);
  Serial.println(" ");

  while (!mqttClient.connect(broker, 8883)) {
    Serial.print(".");
    delay(5000);
  }
  Serial.println();
  Serial.println("You're connected to the MQTT broker");
  Serial.println();

  // 디바이스가 구독한 주제 
  mqttClient.subscribe("$aws/things/Project_MyMKRWiFi1010/shadow/update/delta");
}

void getDeviceStatus(char* payload) {
  float t = dht.readTemperature();
  const char* leds_state = (led1.getState() == LED_ON)? "ON" : "OFF";
  const char* DC1_state = (DC1.getState() == DC_ON) ? "ON" : "OFF";
  sprintf(payload,"{\"state\":{\"reported\":{\"temperature\":\"%0.2f\",\"LEDS\":\"%s\",\"DC_MOTOR\":\"%s\"}}}",t,leds_state,DC1_state);

 if(t > 25){
    led1.off();
    led2.off();
    led3.off();
    led4.off();
    led5.off();
    DC1.on();
  }

  else{
    led1.on();
    led2.on();
    led3.on();
    led4.on();
    led5.on();
    DC1.off();
  }
  Serial.println("Now state , and send message to AWS");
  Serial.print("temperature : ");
  Serial.print(t);
  Serial.print("\tLEDS : ");
  Serial.print(leds_state);
  Serial.print("\tDC모터 : ");
  Serial.print(DC1_state);
  Serial.println();
}

void sendMessage(char* payload) {
  char TOPIC_NAME[]= "$aws/things/Project_MyMKRWiFi1010/shadow/update";

  /*
  MQTT Client로 메시지를 보낸다. 그렇다면 PC는 $aws/things/Project_MyMKRWiFi1010/shadow/update
  구독을 통해, 데이터가 들어오는지 확인한다.
  */
  mqttClient.beginMessage(TOPIC_NAME);
  mqttClient.print(payload);
  mqttClient.endMessage();
}

//메시지 받았을 때 호출되는 콜백함수, 아까 구독한 주제만 허용
void onMessageReceived(int messageSize) {

  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());

  // 받은 메시지 버퍼에 저장
  char buffer[512] ;
  int count=0;
  // mqttClient가 받아온 문자열 있으면 반복 없으면 반복문 탈출
  while (mqttClient.available()) {
     buffer[count++] = (char)mqttClient.read();
  }

  buffer[count]='\0'; // 버퍼의 마지막에 null 캐릭터 삽입
  Serial.println(buffer);
  Serial.println();

  // JSon 형식의 문자열인 buffer를 파싱하여 필요한 값을 얻어옴.
  // 디바이스가 구독한 토픽이 $aws/things/MyMKRWiFi1010/shadow/update/delta 이므로,
  // JSon 문자열 형식은 다음과 같다.
  // {
  //    "version":391,
  //    "timestamp":1572784097,
  //    "state":{
  //        "LED":"ON"
  //    },
  //    "metadata":{
  //        "LED":{
  //          "timestamp":15727840
  //         }
  //    }
  // }
  //
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, buffer);
  JsonObject root = doc.as<JsonObject>();
  JsonObject state = root["state"];
  const char* leds_state = state["LEDS"];
  const char* DC1_state = state["DC_MOTOR"];
  Serial.println(leds_state);
  Serial.println(DC1_state);
  
  char payload[512];

  if (strcmp(leds_state,"ON")==0 &&strcmp(DC1_state,"OFF")==0) {
    led1.on();
    led2.on();
    led3.on();
    led4.on();
    led5.on();
    DC1.off();
    sprintf(payload,"{\"state\":{\"reported\":{\"LEDS\":\"%s\",\"DC_MOTOR\":\"%s\"}}}","ON","OFF");
    sendMessage(payload);
    delay(10000);
    
    
  } else if (strcmp(leds_state,"OFF")==0 &&strcmp(DC1_state,"ON")==0){
    led1.off();
    led2.off();
    led3.off();
    led4.off();
    led5.off();
    DC1.on();
    sprintf(payload,"{\"state\":{\"reported\":{\"LEDS\":\"%s\",\"DC_MOTOR\":\"%s\"}}}","OFF","ON");
    sendMessage(payload);
    delay(10000);
  }
  
 
}