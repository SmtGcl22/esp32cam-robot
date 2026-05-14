/*
 * ═══════════════════════════════════════════════════════════════
 *  KESİS-ROBOT — Ana ESP32 Alıcı (ESP-NOW + Mecanum Kontrol)
 *               + DHT11 Sıcaklık/Nem + MQ Gaz Sensörü
 * ═══════════════════════════════════════════════════════════════
 *  Board     : ESP32 DevKit v1
 *  Sürücü    : 2x TB6612FNG
 *  Tekerlekler: 4x Mecanum
 *  Protokol  : ESP-NOW çift yönlü (ESP32-CAM ile)
 *  Sensörler : DHT11 (GPIO5), MQ Gaz (GPIO34)
 * ═══════════════════════════════════════════════════════════════
 *  Mecanum Kinematik:
 *    FL = Y + X + Rot    (Sol Ön)
 *    BL = Y - X + Rot    (Sol Arka)
 *    FR = Y - X - Rot    (Sağ Ön)
 *    BR = Y + X - Rot    (Sağ Arka)
 * ═══════════════════════════════════════════════════════════════
 */

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// ═══════════════════════════════════════
//  TB6612FNG Motor Sürücü Pin Tanımları
// ═══════════════════════════════════════
#define STBY       23   // Standby (her iki sürücü için ortak)

// SOL ÖN MOTOR (Front Left)
#define AIN1_FL    33
#define AIN2_FL    25
#define PWM_FL     18

// SOL ARKA MOTOR (Back Left)
#define BIN1_BL    26
#define BIN2_BL    27
#define PWM_BL     19

// SAĞ ÖN MOTOR (Front Right)
#define AIN1_FR    14
#define AIN2_FR     4
#define PWM_FR     21

// SAĞ ARKA MOTOR (Back Right)
#define BIN1_BR    13
#define BIN2_BR    15
#define PWM_BR     22

#define BUZZER_PIN 32   // Korna (Buzzer) Pini

// ═══════════════════════════════════════
//  Sensör Pin Tanımları
// ═══════════════════════════════════════
#define MQ_PIN     34       // MQ Gaz sensörü Analog çıkışı (ADC1)

// ═══════════════════════════════════════
//  Sabitler
// ═══════════════════════════════════════
// PWM Ayarları
#define MOTOR_FREQ  1000
#define MOTOR_RES   8     // 8-bit → 0-255

// ESP-NOW WiFi kanal (ESP32-CAM ile aynı olmalı)
#define ESPNOW_CHANNEL 1

// Güvenlik timeout (ms) — bu süre içinde veri gelmezse dur
#define TIMEOUT_MS  500

// Sensör okuma aralığı (ms)
#define SENSOR_INTERVAL 1000

// ═══════════════════════════════════════
//  ESP-NOW Kontrol Verisi Yapısı (CAM → Alıcı)
// ═══════════════════════════════════════
typedef struct __attribute__((packed)) {
  int8_t  x;      // -100..100 strafe (sola/sağa kayma)
  int8_t  y;      // -100..100 ileri/geri
  int8_t  rot;    // -100..100 dönüş
  uint8_t speed;  // 0..255 max hız
  uint8_t horn;   // 0=Kapalı, 1=Açık
} ControlData;

// ═══════════════════════════════════════
//  Sensör Verisi Yapısı (Alıcı → CAM)
// ═══════════════════════════════════════
typedef struct __attribute__((packed)) {
  int16_t gasValue;     // 0-4095 (ADC ham değer)
} SensorData;

// ═══════════════════════════════════════
//  Global Değişkenler
// ═══════════════════════════════════════
ControlData ctrlData = {0, 0, 0, 200, 0};
SensorData  sensorData = {0};
unsigned long lastRecvTime = 0;
bool dataReceived = false;

// ESP32-CAM MAC adresi (ilk veri geldiğinde yakalanacak)
uint8_t camMAC[6] = {0};
bool camPeerAdded = false;

// Sensör zamanlayıcı
unsigned long lastSensorRead = 0;

// ═══════════════════════════════════════
//  Tek Motor Sürme
// ═══════════════════════════════════════
void driveMotor(int in1, int in2, int pwmPin, int value, uint8_t maxSpeed) {
  // value: -100..100
  // Pozitif = ileri, Negatif = geri
  int pwm = map(abs(value), 0, 100, 0, maxSpeed);

  if (value > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else if (value < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    pwm = 0;
  }

  ledcWrite(pwmPin, pwm);
}

// ═══════════════════════════════════════
//  Mecanum Kinematik Hesaplama ve Sürme
// ═══════════════════════════════════════
void mecanumDrive(int8_t x, int8_t y, int8_t rot, uint8_t maxSpeed) {
  // Kinematik hesaplama
  int fl = y + x + rot;   // Sol Ön
  int bl = y - x + rot;   // Sol Arka
  int fr = y - x - rot;   // Sağ Ön
  int br = y + x - rot;   // Sağ Arka

  // Normalizasyon: Eğer herhangi bir değer 100'ü aşarsa oranla
  int maxVal = max(max(abs(fl), abs(bl)), max(abs(fr), abs(br)));
  if (maxVal > 100) {
    fl = (int)((long)fl * 100 / maxVal);
    bl = (int)((long)bl * 100 / maxVal);
    fr = (int)((long)fr * 100 / maxVal);
    br = (int)((long)br * 100 / maxVal);
  }

  // Motorları sür
  driveMotor(AIN1_FL, AIN2_FL, PWM_FL, fl, maxSpeed);
  driveMotor(BIN1_BL, BIN2_BL, PWM_BL, bl, maxSpeed);
  driveMotor(AIN1_FR, AIN2_FR, PWM_FR, fr, maxSpeed);
  driveMotor(BIN1_BR, BIN2_BR, PWM_BR, br, maxSpeed);

  // Debug çıktısı
  Serial.printf("[MECANUM] X:%4d Y:%4d R:%4d → FL:%4d BL:%4d FR:%4d BR:%4d | Spd:%d\n",
    x, y, rot, fl, bl, fr, br, maxSpeed);
}

// ═══════════════════════════════════════
//  Tüm Motorları Durdur
// ═══════════════════════════════════════
void stopAllMotors() {
  driveMotor(AIN1_FL, AIN2_FL, PWM_FL, 0, 0);
  driveMotor(BIN1_BL, BIN2_BL, PWM_BL, 0, 0);
  driveMotor(AIN1_FR, AIN2_FR, PWM_FR, 0, 0);
  driveMotor(BIN1_BR, BIN2_BR, PWM_BR, 0, 0);
}

// ═══════════════════════════════════════
//  ESP-NOW Veri Alım Callback
// ═══════════════════════════════════════
void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len == sizeof(ControlData)) {
    memcpy(&ctrlData, data, sizeof(ControlData));
    lastRecvTime = millis();
    dataReceived = true;

    // İlk veri geldiğinde ESP32-CAM'in MAC adresini yakala ve peer olarak ekle
    if (!camPeerAdded && info->src_addr) {
      memcpy(camMAC, info->src_addr, 6);
      esp_now_peer_info_t peerInfo = {};
      memcpy(peerInfo.peer_addr, camMAC, 6);
      peerInfo.channel = 0;
      peerInfo.encrypt = false;
      peerInfo.ifidx = WIFI_IF_STA;
      if (esp_now_add_peer(&peerInfo) == ESP_OK) {
        camPeerAdded = true;
        Serial.printf("[OK] ESP32-CAM peer eklendi: %02X:%02X:%02X:%02X:%02X:%02X\n",
          camMAC[0], camMAC[1], camMAC[2], camMAC[3], camMAC[4], camMAC[5]);
      }
    }
  }
}

// ═══════════════════════════════════════
//  ESP-NOW Gönderim Callback
// ═══════════════════════════════════════
void onSensorSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  // Sensör verisi gönderim durumu (opsiyonel debug)
}

// ═══════════════════════════════════════
//  Sensör Okuma ve Gönderim
// ═══════════════════════════════════════
void readAndSendSensors() {
  // MQ Gaz sensörü oku (ADC1 — WiFi ile çakışmaz)
  int g = analogRead(MQ_PIN);
  sensorData.gasValue = (int16_t)g;

  Serial.printf("[SENSOR] Gaz:%d\n", sensorData.gasValue);

  // ESP32-CAM'e gönder (peer eklenmişse)
  if (camPeerAdded) {
    esp_err_t result = esp_now_send(camMAC, (uint8_t *)&sensorData, sizeof(sensorData));
    if (result != ESP_OK) {
      Serial.println("[HATA] Sensör verisi gönderilemedi!");
    }
  }
}

// ═══════════════════════════════════════
//  SETUP
// ═══════════════════════════════════════
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("═══════════════════════════════════════");
  Serial.println("   KESİS-ROBOT — ESP-NOW Alıcı");
  Serial.println("   Mecanum 4WD + Sensörler");
  Serial.println("═══════════════════════════════════════");

  // ── Standby pin ──
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  // ── Buzzer pin ──
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // ── Motor yön pinleri ──
  pinMode(AIN1_FL, OUTPUT); pinMode(AIN2_FL, OUTPUT);
  pinMode(BIN1_BL, OUTPUT); pinMode(BIN2_BL, OUTPUT);
  pinMode(AIN1_FR, OUTPUT); pinMode(AIN2_FR, OUTPUT);
  pinMode(BIN1_BR, OUTPUT); pinMode(BIN2_BR, OUTPUT);

  // ── PWM kanalları ──
  ledcAttach(PWM_FL, MOTOR_FREQ, MOTOR_RES);
  ledcAttach(PWM_BL, MOTOR_FREQ, MOTOR_RES);
  ledcAttach(PWM_FR, MOTOR_FREQ, MOTOR_RES);
  ledcAttach(PWM_BR, MOTOR_FREQ, MOTOR_RES);

  stopAllMotors();
  Serial.println("[OK] Motor pinleri hazır (TB6612 x2)");

  // ── Sensörler başlat ──
  pinMode(MQ_PIN, INPUT);
  Serial.println("[OK] MQ Gaz (GPIO34) hazır");

  // ── WiFi STA modu (sadece ESP-NOW için) ──
  WiFi.mode(WIFI_STA);
  delay(500); // WiFi modülünün tam başlamasını bekle

  // ESP32-CAM ile aynı kanala ayarla
  esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

  // MAC adresini yazdır
  Serial.print("[OK] WiFi STA — MAC: ");
  Serial.println(WiFi.macAddress());

  // ── ESP-NOW başlat ──
  if (esp_now_init() != ESP_OK) {
    Serial.println("[HATA] ESP-NOW başlatılamadı!");
    while (true) { delay(1000); }
  }

  esp_now_register_recv_cb(onDataRecv);
  esp_now_register_send_cb(onSensorSent);
  Serial.println("[OK] ESP-NOW çift yönlü hazır — Komut bekleniyor...");

  Serial.println("═══════════════════════════════════════");
  Serial.println("   HAZIR — ESP32-CAM'den veri bekleniyor");
  Serial.println("═══════════════════════════════════════");

  lastRecvTime = millis();
  lastSensorRead = millis();
}

// ═══════════════════════════════════════
//  LOOP
// ═══════════════════════════════════════
void loop() {
  // Veri geldiğinde motorları sür
  if (dataReceived) {
    dataReceived = false;
    mecanumDrive(ctrlData.x, ctrlData.y, ctrlData.rot, ctrlData.speed);
    digitalWrite(BUZZER_PIN, ctrlData.horn ? HIGH : LOW);
  }

  // Güvenlik: Belirli süre veri gelmezse motorları durdur
  if (millis() - lastRecvTime > TIMEOUT_MS) {
    stopAllMotors();
    digitalWrite(BUZZER_PIN, LOW); // Kornayı sustur
    // Her 2 saniyede bir uyarı yazdır
    static unsigned long lastWarn = 0;
    if (millis() - lastWarn > 2000) {
      Serial.println("[UYARI] ESP-NOW sinyal yok — motorlar durduruldu");
      lastWarn = millis();
    }
  }

  // ── Sensör okuma (her SENSOR_INTERVAL ms'de bir) ──
  if (millis() - lastSensorRead >= SENSOR_INTERVAL) {
    lastSensorRead = millis();
    readAndSendSensors();
  }

  delay(5); // CPU rahatlat
}
