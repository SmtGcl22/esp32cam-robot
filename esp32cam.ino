/*
 * ═══════════════════════════════════════════════════════════════
 *  KESİS-ROBOT — ESP32-CAM Kamera + Web Sunucu + ESP-NOW Gönderici
 * ═══════════════════════════════════════════════════════════════
 *  Board     : ESP32-CAM (AI-Thinker)
 *  Kamera    : OV2640
 *  WiFi      : AP modu — SSID: KesisRobot-CAM / Şifre: kesis2026
 *  Protokol  : ESP-NOW → Ana ESP32 (E0:8C:FE:30:D8:54)
 *  Arayüz    : MJPEG stream + Sanal Joystick
 * ═══════════════════════════════════════════════════════════════
 */

#include "esp_camera.h"
#include "esp_http_server.h"
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "web_page.h"

// ═══════════════════════════════════════
//  AI-Thinker Kamera Pin Tanımları
// ═══════════════════════════════════════
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

// Flash LED
#define FLASH_PIN 4

// ═══════════════════════════════════════
//  WiFi AP Ayarları
// ═══════════════════════════════════════
const char *AP_SSID = "KesisRobot-CAM";
const char *AP_PASS = "kesis2026";
const int AP_CHANNEL = 1;

// ═══════════════════════════════════════
//  ESP-NOW — Ana ESP32 MAC Adresi
// ═══════════════════════════════════════
uint8_t receiverMAC[] = {0xE0, 0x8C, 0xFE, 0x30, 0xD8, 0x54};

// Kontrol verisi yapısı (4 byte)
typedef struct __attribute__((packed)) {
  int8_t x;      // -100..100 strafe
  int8_t y;      // -100..100 ileri/geri
  int8_t rot;    // -100..100 dönüş
  uint8_t speed; // 0..255 max hız
} ControlData;

ControlData ctrlData = {0, 0, 0, 200};

// HTTP sunucu handle'ları
httpd_handle_t stream_httpd = NULL;
httpd_handle_t ctrl_httpd = NULL;

// ═══════════════════════════════════════
//  ESP-NOW Gönderim Callback
// ═══════════════════════════════════════
void onDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.printf("[ESP-NOW TX] %s\n",
                status == ESP_NOW_SEND_SUCCESS ? "BASARILI" : "BASARISIZ");
}

// ═══════════════════════════════════════
//  ESP-NOW Veri Gönder
// ═══════════════════════════════════════
void sendESPNow() {
  esp_now_send(receiverMAC, (uint8_t *)&ctrlData, sizeof(ctrlData));
}

// ═══════════════════════════════════════
//  Kamera Başlatma
// ═══════════════════════════════════════
bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;

  // PSRAM varsa daha yüksek çözünürlük
  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA; // 640x480
    config.jpeg_quality = 12;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QVGA; // 320x240
    config.jpeg_quality = 15;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[HATA] Kamera başlatılamadı: 0x%x\n", err);
    return false;
  }

  // Görüntü iyileştirme
  sensor_t *s = esp_camera_sensor_get();
  if (s) {
    s->set_brightness(s, 1);
    s->set_contrast(s, 1);
    s->set_saturation(s, 0);
  }

  Serial.println("[OK] Kamera başlatıldı");
  return true;
}

// ═══════════════════════════════════════
//  MJPEG Stream Handler
// ═══════════════════════════════════════
#define PART_BOUNDARY "frame"
static const char *STREAM_CONTENT_TYPE =
    "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *STREAM_PART =
    "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  char partBuf[64];

  res = httpd_resp_set_type(req, STREAM_CONTENT_TYPE);
  if (res != ESP_OK)
    return res;

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("[HATA] Kare yakalanamadı");
      res = ESP_FAIL;
      break;
    }

    size_t hlen = snprintf(partBuf, 64, STREAM_PART, fb->len);

    res = httpd_resp_send_chunk(req, STREAM_BOUNDARY, strlen(STREAM_BOUNDARY));
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, partBuf, hlen);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
    }

    esp_camera_fb_return(fb);

    if (res != ESP_OK)
      break;
  }

  return res;
}

// ═══════════════════════════════════════
//  Ana Sayfa Handler
// ═══════════════════════════════════════
static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, index_html, strlen(index_html));
}

// ═══════════════════════════════════════
//  Kontrol Handler — Joystick verisi alır, ESP-NOW ile gönderir
// ═══════════════════════════════════════
static esp_err_t control_handler(httpd_req_t *req) {
  char buf[100];
  int buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1 && buf_len <= 100) {
    httpd_req_get_url_query_str(req, buf, buf_len);

    char param[8];
    if (httpd_query_key_value(buf, "x", param, sizeof(param)) == ESP_OK)
      ctrlData.x = (int8_t)constrain(atoi(param), -100, 100);
    if (httpd_query_key_value(buf, "y", param, sizeof(param)) == ESP_OK)
      ctrlData.y = (int8_t)constrain(atoi(param), -100, 100);
    if (httpd_query_key_value(buf, "r", param, sizeof(param)) == ESP_OK)
      ctrlData.rot = (int8_t)constrain(atoi(param), -100, 100);
    if (httpd_query_key_value(buf, "s", param, sizeof(param)) == ESP_OK)
      ctrlData.speed = (uint8_t)constrain(atoi(param), 0, 255);

    // ESP-NOW ile gönder
    sendESPNow();

    Serial.printf("[KONTROL] X:%d Y:%d R:%d S:%d\n", ctrlData.x, ctrlData.y,
                  ctrlData.rot, ctrlData.speed);
  }

  httpd_resp_set_type(req, "text/plain");
  return httpd_resp_send(req, "OK", 2);
}

// ═══════════════════════════════════════
//  Flash LED Handler
// ═══════════════════════════════════════
static esp_err_t flash_handler(httpd_req_t *req) {
  char buf[16];
  int buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1 && buf_len <= 16) {
    httpd_req_get_url_query_str(req, buf, buf_len);
    char param[4];
    if (httpd_query_key_value(buf, "v", param, sizeof(param)) == ESP_OK) {
      digitalWrite(FLASH_PIN, atoi(param) ? HIGH : LOW);
    }
  }
  httpd_resp_set_type(req, "text/plain");
  return httpd_resp_send(req, "OK", 2);
}

// ═══════════════════════════════════════
//  HTTP Sunucu Başlatma
// ═══════════════════════════════════════
void startWebServer() {
  // Stream sunucusu (port 81)
  httpd_config_t stream_config = HTTPD_DEFAULT_CONFIG();
  stream_config.server_port = 81;
  stream_config.ctrl_port = 32769;

  if (httpd_start(&stream_httpd, &stream_config) == ESP_OK) {
    httpd_uri_t stream_uri = {.uri = "/stream",
                              .method = HTTP_GET,
                              .handler = stream_handler,
                              .user_ctx = NULL};
    httpd_register_uri_handler(stream_httpd, &stream_uri);
    Serial.println("[OK] Stream sunucu başlatıldı (port 81)");
  }

  // Kontrol sunucusu (port 80)
  httpd_config_t ctrl_config = HTTPD_DEFAULT_CONFIG();
  ctrl_config.server_port = 80;

  if (httpd_start(&ctrl_httpd, &ctrl_config) == ESP_OK) {
    httpd_uri_t index_uri = {.uri = "/",
                             .method = HTTP_GET,
                             .handler = index_handler,
                             .user_ctx = NULL};
    httpd_uri_t control_uri = {.uri = "/control",
                               .method = HTTP_GET,
                               .handler = control_handler,
                               .user_ctx = NULL};
    httpd_uri_t flash_uri = {.uri = "/flash",
                             .method = HTTP_GET,
                             .handler = flash_handler,
                             .user_ctx = NULL};
    httpd_register_uri_handler(ctrl_httpd, &index_uri);
    httpd_register_uri_handler(ctrl_httpd, &control_uri);
    httpd_register_uri_handler(ctrl_httpd, &flash_uri);
    Serial.println("[OK] Kontrol sunucu başlatıldı (port 80)");
  }
}

// ═══════════════════════════════════════
//  ESP-NOW Başlatma
// ═══════════════════════════════════════
void initESPNow() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("[HATA] ESP-NOW başlatılamadı!");
    return;
  }

  esp_now_register_send_cb(onDataSent);

  // Alıcı peer ekle
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0; // 0 = mevcut kanalı kullan
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_AP; // AP modunda çalıştığımız için

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("[HATA] ESP-NOW peer eklenemedi!");
    return;
  }

  Serial.printf("[OK] ESP-NOW hazır → %02X:%02X:%02X:%02X:%02X:%02X\n",
                receiverMAC[0], receiverMAC[1], receiverMAC[2], receiverMAC[3],
                receiverMAC[4], receiverMAC[5]);
}

// ═══════════════════════════════════════
//  SETUP
// ═══════════════════════════════════════
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("═══════════════════════════════════════");
  Serial.println("   KESİS-ROBOT CAM — Başlatılıyor...");
  Serial.println("═══════════════════════════════════════");

  // Flash LED pin
  pinMode(FLASH_PIN, OUTPUT);
  digitalWrite(FLASH_PIN, LOW);

  // Kamera başlat
  if (!initCamera()) {
    Serial.println("[HATA] Kamera hatası! Yeniden başlatılıyor...");
    delay(2000);
    ESP.restart();
  }

  // WiFi AP modu
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS, AP_CHANNEL);
  delay(300); // AP stabilizasyonu

  IPAddress IP = WiFi.softAPIP();
  Serial.println("[OK] WiFi AP başlatıldı");
  Serial.printf("     SSID   : %s\n", AP_SSID);
  Serial.printf("     Şifre  : %s\n", AP_PASS);
  Serial.printf("     Kanal  : %d\n", AP_CHANNEL);
  Serial.printf("     IP     : %s\n", IP.toString().c_str());

  // ESP-NOW başlat
  initESPNow();

  // Web sunucu başlat
  startWebServer();

  Serial.println("═══════════════════════════════════════");
  Serial.println("   HAZIR — Tarayıcıda aç:");
  Serial.printf("   http://%s\n", IP.toString().c_str());
  Serial.println("═══════════════════════════════════════");
}

// ═══════════════════════════════════════
//  LOOP
// ═══════════════════════════════════════
void loop() { delay(10); }
