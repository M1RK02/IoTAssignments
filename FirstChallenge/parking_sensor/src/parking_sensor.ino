// Libraries for Wifi and communication
#include <WiFi.h>
#include <esp_now.h>

// HC-SR04 ultrasonic distance sensor connections
#define PIN_TRIG 12
#define PIN_ECHO 14

// Leader person code = 10811404
// X = 04 % 50 + 5 = 9
#define DEEP_SLEEP_DURATION 9

// Used to capture estimates of time in diffrent states
#define COLLECTING_DATA true

uint8_t broadcastAddress[] = {0x8C, 0xAA, 0xB5, 0x84, 0xFB, 0x90};

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Ok" : "Error");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *data, int len) {
  char receivedString[len];
  memcpy(receivedString, data, len);
  Serial.print("Message received: ");
  Serial.println(String(receivedString));
}

void Timestamp() {
  if(COLLECTING_DATA){
    Serial.println(micros());
  }
}

void setup() {
  Serial.begin(115200);
  Timestamp();

  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_TRIG, OUTPUT);

  Timestamp();
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  int duration = pulseIn(PIN_ECHO, HIGH);
  int distance = duration / 58;
  Timestamp();

  String status = "FREE";
  if(distance <= 50) {
    status = "OCCUPIED";
  }

  Timestamp();
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_2dBm);

  esp_now_init();
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  Serial.print("Message to be sent: ");
  Serial.println(status);
  Timestamp();
  esp_now_send(broadcastAddress, (uint8_t*)status.c_str(), status.length() + 1);
  Timestamp();

  // Needed to receive the message in the simulation
  // delay(100);

  WiFi.mode(WIFI_OFF);
  Timestamp();

  esp_sleep_enable_timer_wakeup(DEEP_SLEEP_DURATION * 1000000);

  Timestamp();
  
  Serial.flush();
  esp_deep_sleep_start();
}

void loop() {
  // This won't be called
}