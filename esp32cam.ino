/*
 * ═══════════════════════════════════════════════════════════════
 *  KESİS-ROBOT — ESP32-CAM Kamera + Web Sunucu + ESP-NOW Çift Yönlü
 *               + Otonom Renk Takip + HC-SR04 Mesafe + Telemetri
 * ═══════════════════════════════════════════════════════════════
 *  Board     : ESP32-CAM (AI-Thinker)
 *  Kamera    : OV2640
 *  WiFi      : AP modu — SSID: KesisRobot-CAM / Şifre: kesis2026
 *  Protokol  : ESP-NOW çift yönlü ↔ Ana ESP32 (E0:8C:FE:30:D8:54)
 *  Sensörler : HC-SR04 Mesafe (GPIO13/GPIO2)
 *  Arayüz    : MJPEG stream + Joystick + Otonom + Telemetri Paneli
 * ═══════════════════════════════════════════════════════════════
 */

#include "esp_camera.h"
#include "esp_http_server.h"
#include "img_converters.h"
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
//  Pan-Tilt Servo Pin Tanımları
// ═══════════════════════════════════════
#define SERVO_PAN_PIN   14   // Yatay (sağ-sol)
#define SERVO_TILT_PIN  12   // Dikey (yukarı-aşağı)

// Servo PWM ayarları
#define SERVO_FREQ      50   // 50Hz (standart servo)
#define SERVO_RES       16   // 16-bit çözünürlük

// Servo açı limitleri
#define PAN_MIN    0
#define PAN_MAX    180
#define PAN_CENTER 90
#define TILT_MIN   0
#define TILT_MAX   130
#define TILT_CENTER 90

// Servo mevcut açıları
volatile int panAngle  = PAN_CENTER;
volatile int tiltAngle = TILT_CENTER;

// ═══════════════════════════════════════
//  HC-SR04 Mesafe Sensörü Pin Tanımları
// ═══════════════════════════════════════
#define TRIG_PIN   13   // Ultrasonik Trig çıkışı
#define ECHO_PIN    2   // Ultrasonik Echo girişi

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
  uint8_t horn;  // 0=Kapalı, 1=Açık
} ControlData;

ControlData ctrlData = {0, 0, 0, 200, 0};

// ═══════════════════════════════════════
//  Sensör Verisi Yapısı (Alıcı → CAM)
// ═══════════════════════════════════════
typedef struct __attribute__((packed)) {
  int16_t gasValue;     // 0-4095 (ADC ham değer)
} SensorData;

// Alıcıdan gelen sensör verileri
volatile int recvGas  = 0;

// HC-SR04 mesafe ölçümü
volatile float distanceCm = -1;
unsigned long lastDistRead = 0;
#define DIST_INTERVAL 200  // 200ms'de bir ölç

// HTTP sunucu handle'ları
httpd_handle_t stream_httpd = NULL;
httpd_handle_t ctrl_httpd = NULL;

// ═══════════════════════════════════════
//  Otonom Renk Takip — Tanımlar
// ═══════════════════════════════════════
// Hedef renk sabitleri
#define COLOR_RED    0
#define COLOR_BLUE   1
#define COLOR_YELLOW 2

// İşleme çözünürlüğü (QVGA)
#define PROC_WIDTH   320
#define PROC_HEIGHT  240

// Piksel atlama (her N. pikseli tara → hız kazancı)
#define SCAN_STRIDE  4

// Minimum piksel sayısı — bunun altındaysa "top bulunamadı"
#define MIN_PIXEL_COUNT  25

// Otonom hız ayarı
#define AUTO_SPEED   160

// Otonom durum değişkenleri
volatile bool   autoMode      = false;
volatile int    targetColor   = COLOR_RED;
volatile bool   targetFound   = false;
volatile int    targetCX      = 0;   // hedef merkez X (0..PROC_WIDTH)
volatile int    targetCY      = 0;   // hedef merkez Y (0..PROC_HEIGHT)
volatile int    targetArea    = 0;   // tespit edilen piksel sayısı
volatile int    detFPS        = 0;   // algılama FPS

// FreeRTOS task handle
TaskHandle_t detectTaskHandle = NULL;

// ═══════════════════════════════════════
//  Renk Eşleştirme Fonksiyonu
// ═══════════════════════════════════════
// Her piksel için RGB değerlerini alıp hedef renge uyup uymadığını kontrol eder
bool isTargetColor(uint8_t r, uint8_t g, uint8_t b, int color) {
  switch (color) {
    case COLOR_RED:
      // Kırmızı: R baskın, G ve B düşük
      return (r > 110 && g < 90 && b < 90 && r > (g + 40));

    case COLOR_BLUE:
      // Mavi: B baskın — OV2640 mavi kanalı zayıf olduğu için gevşek eşik
      return (b > 80 && r < 100 && g < 120 && b > r && b > (g - 20));

    case COLOR_YELLOW:
      // Sarı: R ve G yüksek, B düşük — uzaktan da algılansın
      return (r > 100 && g > 80 && b < 100 && (r + g) > (b * 3 + 100));

    default:
      return false;
  }
}

// ═══════════════════════════════════════
//  Görüntü İşleme — Renk Blob Tarama
// ═══════════════════════════════════════
void detectColorBlob(uint8_t *rgb, int width, int height) {
  long sumX = 0, sumY = 0;
  int count = 0;

  for (int y = 0; y < height; y += SCAN_STRIDE) {
    for (int x = 0; x < width; x += SCAN_STRIDE) {
      int idx = (y * width + x) * 3;
      uint8_t r = rgb[idx];
      uint8_t g = rgb[idx + 1];
      uint8_t b = rgb[idx + 2];

      if (isTargetColor(r, g, b, targetColor)) {
        sumX += x;
        sumY += y;
        count++;
      }
    }
  }

  if (count >= MIN_PIXEL_COUNT) {
    targetFound = true;
    targetCX = sumX / count;
    targetCY = sumY / count;
    targetArea = count;
  } else {
    targetFound = false;
    targetCX = 0;
    targetCY = 0;
    targetArea = 0;
  }
}

// ═══════════════════════════════════════
//  Otonom Takip — Motor Komut Hesaplama
// ═══════════════════════════════════════
void autonomousControl() {
  if (!targetFound) {
    // Top bulunamadı → yavaşça yerinde dön (arama modu)
    ctrlData.x   = 0;
    ctrlData.y   = 0;
    ctrlData.rot = 30;  // Yavaş sağa dönüş ile topı ara
    ctrlData.speed = AUTO_SPEED / 2;
    sendESPNow();
    return;
  }

  // Ekranın merkezi
  int centerX = PROC_WIDTH / 2;   // 160
  int centerY = PROC_HEIGHT / 2;  // 120

  // Sapma hesapla (-100..+100 aralığına normalize)
  int errorX = targetCX - centerX; // pozitif = sağda
  int errorY = centerY - targetCY; // pozitif = yukarıda (uzakta)

  // Dönüş komutu: top sağdaysa sağa dön
  int rotCmd = constrain(errorX * 100 / centerX, -100, 100);

  // İleri/geri: top ekranın üstündeyse (uzakta) ileri git
  // Top ekranın altındaysa (yakında) yavaşla veya dur
  int fwdCmd = 0;
  if (targetArea < 300) {
    // Top küçük görünüyor = uzakta → ileri git
    fwdCmd = constrain(40 + (300 - targetArea) / 5, 20, 80);
  } else if (targetArea > 600) {
    // Top çok büyük = çok yakın → geri gel
    fwdCmd = -30;
  }
  // Yakın mesafede ise fwdCmd = 0 (dur)

  ctrlData.x   = 0;
  ctrlData.y   = (int8_t)constrain(fwdCmd, -100, 100);
  ctrlData.rot = (int8_t)constrain(rotCmd / 2, -60, 60); // dönüşü yumuşat
  ctrlData.speed = AUTO_SPEED;

  sendESPNow();

  Serial.printf("[OTONOM] CX:%d CY:%d Area:%d → Y:%d R:%d\n",
                targetCX, targetCY, targetArea, ctrlData.y, ctrlData.rot);
}

// ═══════════════════════════════════════
//  Algılama FreeRTOS Task
// ═══════════════════════════════════════
void detectTask(void *param) {
  // İşleme için ayrı kamera çözünürlük ayarı
  Serial.println("[OTONOM] Algılama task'ı başladı");

  unsigned long fpsTimer = millis();
  int fpsCount = 0;

  while (true) {
    if (!autoMode) {
      targetFound = false;
      vTaskDelay(pdMS_TO_TICKS(200));
      continue;
    }

    // Kameradan kare al
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      vTaskDelay(pdMS_TO_TICKS(50));
      continue;
    }

    // JPEG → RGB dönüştürme (PSRAM'da)
    uint8_t *rgb_buf = NULL;
    size_t rgb_len = fb->width * fb->height * 3;
    rgb_buf = (uint8_t *)ps_malloc(rgb_len);

    if (rgb_buf) {
      bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, rgb_buf);
      esp_camera_fb_return(fb);

      if (converted) {
        detectColorBlob(rgb_buf, fb->width, fb->height);

        // Otonom kontrol komutu gönder
        autonomousControl();

        fpsCount++;
      }
      free(rgb_buf);
    } else {
      esp_camera_fb_return(fb);
      Serial.println("[HATA] RGB buffer ayrılamadı!");
      vTaskDelay(pdMS_TO_TICKS(500));
    }

    // FPS hesapla
    if (millis() - fpsTimer >= 1000) {
      detFPS = fpsCount;
      fpsCount = 0;
      fpsTimer = millis();
    }

    vTaskDelay(pdMS_TO_TICKS(30)); // ~30 FPS hedef (işleme süresi dahil daha az olacak)
  }
}

// ═══════════════════════════════════════
//  ESP-NOW Gönderim Callback
// ═══════════════════════════════════════
void onDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  // Gönderim durumu (debug gerekirse açılabilir)
}

// ═══════════════════════════════════════
//  ESP-NOW Sensör Verisi Alım Callback
// ═══════════════════════════════════════
void onSensorRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len == sizeof(SensorData)) {
    SensorData sd;
    memcpy(&sd, data, sizeof(SensorData));
    recvGas  = sd.gasValue;
    Serial.printf("[SENSOR RX] Gaz:%d\n", recvGas);
  }
}

// ═══════════════════════════════════════
//  ESP-NOW Veri Gönder
// ═══════════════════════════════════════
void sendESPNow() {
  esp_now_send(receiverMAC, (uint8_t *)&ctrlData, sizeof(ctrlData));
}

// ═══════════════════════════════════════
//  HC-SR04 Mesafe Ölçümü
// ═══════════════════════════════════════
float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000); // 25ms timeout (~4.25m max)
  if (duration == 0) return -1;  // zaman aşımı — engel yok
  return duration * 0.034 / 2.0; // cm cinsinden mesafe
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
    s->set_saturation(s, 2);  // Renk doygunluğu artırıldı (algılama için)
    s->set_whitebal(s, 1);    // Otomatik beyaz dengesi
    s->set_awb_gain(s, 1);
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
    if (httpd_query_key_value(buf, "h", param, sizeof(param)) == ESP_OK)
      ctrlData.horn = (uint8_t)constrain(atoi(param), 0, 1);

    // ESP-NOW ile gönder
    sendESPNow();

    Serial.printf("[KONTROL] X:%d Y:%d R:%d S:%d H:%d\n", ctrlData.x, ctrlData.y,
                  ctrlData.rot, ctrlData.speed, ctrlData.horn);
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
//  Otonom Mod Handler — /automode?mode=1&color=0
// ═══════════════════════════════════════
static esp_err_t automode_handler(httpd_req_t *req) {
  char buf[64];
  int buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1 && buf_len <= 64) {
    httpd_req_get_url_query_str(req, buf, buf_len);
    char param[8];
    if (httpd_query_key_value(buf, "mode", param, sizeof(param)) == ESP_OK) {
      bool newMode = atoi(param) != 0;
      if (newMode != autoMode) {
        autoMode = newMode;
        if (!autoMode) {
          // Otonom mod kapatıldı → motorları durdur
          ctrlData.x = 0;
          ctrlData.y = 0;
          ctrlData.rot = 0;
          ctrlData.speed = 200;
          sendESPNow();
          targetFound = false;
        }
        Serial.printf("[OTONOM] Mod: %s\n", autoMode ? "ACIK" : "KAPALI");
      }
    }
    if (httpd_query_key_value(buf, "color", param, sizeof(param)) == ESP_OK) {
      targetColor = constrain(atoi(param), 0, 2);
      const char* colorNames[] = {"KIRMIZI", "MAVI", "SARI"};
      Serial.printf("[OTONOM] Hedef renk: %s\n", colorNames[targetColor]);
    }
  }
  httpd_resp_set_type(req, "text/plain");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, "OK", 2);
}

// ═══════════════════════════════════════
//  Hedef Bilgi Handler — /target (JSON)
// ═══════════════════════════════════════
static esp_err_t target_handler(httpd_req_t *req) {
  char json[128];
  snprintf(json, sizeof(json),
    "{\"found\":%s,\"cx\":%d,\"cy\":%d,\"area\":%d,\"color\":%d,\"auto\":%s,\"fps\":%d}",
    targetFound ? "true" : "false",
    targetCX, targetCY, targetArea,
    targetColor,
    autoMode ? "true" : "false",
    detFPS);

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json, strlen(json));
}

// ═══════════════════════════════════════
//  Servo Açı → PWM Duty Hesaplama
// ═══════════════════════════════════════
uint32_t angleToDuty(int angle) {
  // 0° = 500µs, 180° = 2500µs, periyot = 20000µs
  // 16-bit: duty = (pulse_us / 20000) * 65535
  uint32_t pulse = map(angle, 0, 180, 500, 2500);
  return (uint32_t)((uint64_t)pulse * 65535 / 20000);
}

void setServo(int pin, int angle) {
  ledcWrite(pin, angleToDuty(angle));
}

// ═══════════════════════════════════════
//  Servo Handler — /servo?pan=90&tilt=90
// ═══════════════════════════════════════
static esp_err_t servo_handler(httpd_req_t *req) {
  char buf[64];
  int buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1 && buf_len <= 64) {
    httpd_req_get_url_query_str(req, buf, buf_len);
    char param[8];
    if (httpd_query_key_value(buf, "pan", param, sizeof(param)) == ESP_OK) {
      panAngle = constrain(atoi(param), PAN_MIN, PAN_MAX);
      setServo(SERVO_PAN_PIN, panAngle);
    }
    if (httpd_query_key_value(buf, "tilt", param, sizeof(param)) == ESP_OK) {
      tiltAngle = constrain(atoi(param), TILT_MIN, TILT_MAX);
      setServo(SERVO_TILT_PIN, tiltAngle);
    }
    Serial.printf("[SERVO] Pan:%d° Tilt:%d°\n", panAngle, tiltAngle);
  }
  httpd_resp_set_type(req, "text/plain");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, "OK", 2);
}

// ═══════════════════════════════════════
//  Sensör Handler — /sensors (JSON)
// ═══════════════════════════════════════
static esp_err_t sensors_handler(httpd_req_t *req) {
  char json[200];
  snprintf(json, sizeof(json),
    "{\"dist\":%.1f,\"gas\":%d,\"distAlert\":%s,\"gasAlert\":%s}",
    distanceCm, recvGas,
    (distanceCm > 0 && distanceCm < 15) ? "true" : "false",
    (recvGas > 2000) ? "true" : "false");

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json, strlen(json));
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
  ctrl_config.max_uri_handlers = 10;  // Daha fazla endpoint için

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
    httpd_uri_t automode_uri = {.uri = "/automode",
                                .method = HTTP_GET,
                                .handler = automode_handler,
                                .user_ctx = NULL};
    httpd_uri_t target_uri = {.uri = "/target",
                              .method = HTTP_GET,
                              .handler = target_handler,
                              .user_ctx = NULL};
    httpd_uri_t servo_uri = {.uri = "/servo",
                              .method = HTTP_GET,
                              .handler = servo_handler,
                              .user_ctx = NULL};
    httpd_uri_t sensors_uri = {.uri = "/sensors",
                               .method = HTTP_GET,
                               .handler = sensors_handler,
                               .user_ctx = NULL};
    httpd_register_uri_handler(ctrl_httpd, &index_uri);
    httpd_register_uri_handler(ctrl_httpd, &control_uri);
    httpd_register_uri_handler(ctrl_httpd, &flash_uri);
    httpd_register_uri_handler(ctrl_httpd, &automode_uri);
    httpd_register_uri_handler(ctrl_httpd, &target_uri);
    httpd_register_uri_handler(ctrl_httpd, &servo_uri);
    httpd_register_uri_handler(ctrl_httpd, &sensors_uri);
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
  esp_now_register_recv_cb(onSensorRecv);

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

  // HC-SR04 pinleri
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  Serial.println("[OK] HC-SR04 mesafe sensörü hazır (Trig:13, Echo:2)");

  // Kamera başlat
  if (!initCamera()) {
    Serial.println("[HATA] Kamera hatası! Yeniden başlatılıyor...");
    delay(2000);
    ESP.restart();
  }

  // Servo PWM başlat (Kameradan sonra başlatıyoruz ki LEDC kanalları çakışmasın)
  ledcAttach(SERVO_PAN_PIN, SERVO_FREQ, SERVO_RES);
  ledcAttach(SERVO_TILT_PIN, SERVO_FREQ, SERVO_RES);
  setServo(SERVO_PAN_PIN, PAN_CENTER);
  setServo(SERVO_TILT_PIN, TILT_CENTER);
  Serial.println("[OK] Pan-Tilt servolar başlatıldı (GPIO14, GPIO12)");

  // WiFi AP modu
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS, AP_CHANNEL);
  delay(300); // AP stabilizasyonu
  
  // WiFi güç tasarrufunu kapat (bağlantı kopmalarını önler)
  esp_wifi_set_ps(WIFI_PS_NONE);

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

  // Otonom algılama task'ını başlat (Core 0'da, Core 1 WiFi/HTTP için)
  xTaskCreatePinnedToCore(
    detectTask,          // Fonksiyon
    "DetectTask",        // İsim
    8192,                // Stack boyutu
    NULL,                // Parametre
    1,                   // Öncelik
    &detectTaskHandle,   // Handle
    0                    // Core 0'da çalıştır
  );
  Serial.println("[OK] Otonom algılama task'ı başlatıldı (Core 0)");

  Serial.println("═══════════════════════════════════════");
  Serial.println("   HAZIR — Tarayıcıda aç:");
  Serial.printf("   http://%s\n", IP.toString().c_str());
  Serial.println("   Otonom mod: Web arayüzünden etkinleştir");
  Serial.println("═══════════════════════════════════════");
}

// ═══════════════════════════════════════
//  LOOP
// ═══════════════════════════════════════
void loop() {
  // HC-SR04 mesafe ölçümü (periyodik)
  if (millis() - lastDistRead >= DIST_INTERVAL) {
    lastDistRead = millis();
    float d = readDistance();
    if (d > 0) {
      distanceCm = d;
    }
  }
  delay(10);
}
