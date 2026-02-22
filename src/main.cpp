#include <Arduino.h>
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ========== WiFi 配置 ==========
#define TOY_SSID "funXtoy"  // WiFi名稱
#define TOY_PWD "123456"    // WiFi密碼（至少8位）

// ========== LED配置 (ESP01S只有GPIO0和GPIO2可用) ==========
#define LED_PIN 2           // GPIO2 - WS2812燈帶
#define NUM_LEDS 8          // 8個LED
#define COLOR_ORDER GRB     // WS2812色序
#define CHIPSET WS2812B     // LED晶片類型
CRGBArray<NUM_LEDS> leds;   // LED陣列

// ========== 震動感應器配置 ==========
#define VIBRATION_PIN 0     // GPIO0 - 震動感應器

// ========== 變數 ==========
unsigned long lastVibrationTime = 0;
unsigned long vibrationCooldown = 500;
int brightnesLevel = 0;
int animationMode = 0;
unsigned long animationTimer = 0;
bool autoMode = true;  // 自動模式（由震動觸發）

// ========== Web服務器 ==========
ESP8266WebServer server(80);

// ========== 函數聲明 ==========
void handleVibration();
void updateAnimation();
void rainbowCycle(uint8_t brightness);
void colorPulse(CRGB color, uint8_t brightness);
void randomFlash();
void clearLEDs();
void setAnimationMode(int mode);
void initWiFi();
void handleRoot();
void handleAPI();
void handleSetMode();
void handleSetColor();
void handleToggleAuto();

// ========== HTML前端 ==========
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-TW">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Interactive Toy Control Panel</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: 'Arial', sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
      padding: 20px;
    }
    .container {
      background: white;
      border-radius: 20px;
      padding: 30px;
      box-shadow: 0 10px 40px rgba(0,0,0,0.3);
      max-width: 400px;
      width: 100%;
    }
    h1 { text-align: center; color: #333; margin-bottom: 30px; font-size: 28px; }
    .mode-section { margin-bottom: 25px; }
    .section-title {
      font-size: 14px;
      color: #666;
      text-transform: uppercase;
      letter-spacing: 1px;
      margin-bottom: 12px;
      font-weight: bold;
    }
    .button-group {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 10px;
      margin-bottom: 15px;
    }
    .button-group.full { grid-template-columns: 1fr; }
    button {
      padding: 12px 16px;
      border: none;
      border-radius: 8px;
      font-size: 14px;
      font-weight: bold;
      cursor: pointer;
      transition: all 0.3s ease;
      text-transform: uppercase;
      letter-spacing: 0.5px;
    }
    .mode-btn {
      background: #f0f0f0;
      color: #333;
      border: 2px solid #ddd;
    }
    .mode-btn:hover { background: #e0e0e0; }
    .mode-btn.active {
      background: #667eea;
      color: white;
      border-color: #667eea;
    }
    .action-btn {
      background: #667eea;
      color: white;
    }
    .action-btn:hover { background: #5568d3; }
    .action-btn:active { transform: scale(0.98); }
    .control-section {
      background: #f9f9f9;
      padding: 15px;
      border-radius: 8px;
      margin-bottom: 15px;
    }
    .slider-group {
      display: flex;
      align-items: center;
      gap: 10px;
      margin-bottom: 10px;
    }
    .slider-group label { min-width: 60px; font-size: 12px; }
    input[type="range"] {
      flex: 1;
      height: 6px;
      border-radius: 3px;
      background: #ddd;
      outline: none;
    }
    input[type="range"]::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 16px;
      height: 16px;
      border-radius: 50%;
      background: #667eea;
      cursor: pointer;
    }
    input[type="range"]::-moz-range-thumb {
      width: 16px;
      height: 16px;
      border-radius: 50%;
      background: #667eea;
      cursor: pointer;
      border: none;
    }
    .toggle {
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 10px;
      background: white;
      border-radius: 6px;
      border: 1px solid #ddd;
    }
    .toggle-switch {
      width: 50px;
      height: 28px;
      background: #ccc;
      border-radius: 14px;
      position: relative;
      cursor: pointer;
      transition: background 0.3s;
    }
    .toggle-switch.active { background: #667eea; }
    .toggle-switch::after {
      content: '';
      position: absolute;
      width: 24px;
      height: 24px;
      background: white;
      border-radius: 50%;
      top: 2px;
      left: 2px;
      transition: left 0.3s;
    }
    .toggle-switch.active::after { left: 24px; }
    .lang-buttons {
      display: flex;
      gap: 8px;
      justify-content: center;
      margin-bottom: 20px;
    }
    .lang-btn {
      padding: 6px 12px;
      border: 1px solid #ddd;
      border-radius: 4px;
      background: #fff;
      cursor: pointer;
      font-size: 12px;
      font-weight: bold;
      transition: all 0.3s;
    }
    .lang-btn.active {
      background: #667eea;
      color: white;
      border-color: #667eea;
    }
    .lang-btn:hover { background: #f0f0f0; }
  </style>
</head>
<body>
  <div class="container">
    <div class="lang-buttons">
      <button class="lang-btn active" onclick="setLanguage('en')">English</button>
      <button class="lang-btn" onclick="setLanguage('zh-TW')">繁體中文</button>
      <button class="lang-btn" onclick="setLanguage('zh-CN')">简体中文</button>
    </div>
    
    <h1>Interactive Toy</h1>
    
    <div class="mode-section">
      <div class="section-title" id="modeTitle">Animation Mode</div>
      <div class="button-group">
        <button class="mode-btn active" id="rainbowBtn" onclick="setMode(0)">Rainbow</button>
        <button class="mode-btn" id="flashBtn" onclick="setMode(1)">Flash</button>
        <button class="mode-btn" id="pulseBtn" onclick="setMode(2)">Pulse</button>
      </div>
    </div>
    
    <div class="control-section">
      <div class="slider-group">
        <label id="brightnessLabel">Brightness:</label>
        <input type="range" id="brightness" min="0" max="255" value="255" onchange="updateBrightness()">
        <span id="brightnessValue">255</span>
      </div>
    </div>
    
    <div class="control-section">
      <div class="section-title" id="autoModeTitle">Auto Mode</div>
      <div class="toggle">
        <span id="vibrationLabel">Vibration Trigger</span>
        <div class="toggle-switch active" id="autoToggle" onclick="toggleAutoMode()"></div>
      </div>
    </div>
    
    <button class="action-btn" style="width: 100%;" id="clearBtn" onclick="clearAll()">Clear All LEDs</button>
    
    <div class="status">
      <strong id="statusTitle">Status:</strong> <span id="statusConnected">Connected to WiFi</span><br>
      <span id="modeStatus">Current Mode: Rainbow Cycle</span>
    </div>
  </div>

  <script>
    var currentMode = 0;
    var currentLang = 'en';
    
    // Multi-language translations
    var i18n = {
      'en': {
        'title': 'Interactive Toy',
        'animationMode': 'Animation Mode',
        'rainbow': 'Rainbow',
        'flash': 'Flash',
        'pulse': 'Pulse',
        'brightness': 'Brightness:',
        'autoMode': 'Auto Mode',
        'vibrationTrigger': 'Vibration Trigger',
        'clearLEDs': 'Clear All LEDs',
        'status': 'Status:',
        'connected': 'Connected to WiFi',
        'currentMode': 'Current Mode: ',
        'rainbowCycle': 'Rainbow Cycle',
        'randomFlash': 'Random Flash',
        'colorPulse': 'Color Pulse',
        'cleared': 'LEDs cleared!'
      },
      'zh-TW': {
        'title': '互動玩具',
        'animationMode': '動畫模式',
        'rainbow': '彩虹',
        'flash': '閃爍',
        'pulse': '脈衝',
        'brightness': '亮度:',
        'autoMode': '自動模式',
        'vibrationTrigger': '震動觸發',
        'clearLEDs': '清空所有LED',
        'status': '狀態:',
        'connected': '已連接至WiFi',
        'currentMode': '當前模式: ',
        'rainbowCycle': '彩虹循環',
        'randomFlash': '隨機閃爍',
        'colorPulse': '顏色脈衝',
        'cleared': 'LED已清空！'
      },
      'zh-CN': {
        'title': '互动玩具',
        'animationMode': '动画模式',
        'rainbow': '彩虹',
        'flash': '闪烁',
        'pulse': '脉冲',
        'brightness': '亮度:',
        'autoMode': '自动模式',
        'vibrationTrigger': '振动触发',
        'clearLEDs': '清空所有LED',
        'status': '状态:',
        'connected': '已连接至WiFi',
        'currentMode': '当前模式: ',
        'rainbowCycle': '彩虹循环',
        'randomFlash': '随机闪烁',
        'colorPulse': '颜色脉冲',
        'cleared': 'LED已清空！'
      }
    };
    
    // Get translation
    function t(key) {
      return i18n[currentLang][key] || i18n['en'][key] || key;
    }
    
    // Update UI with current language
    function updateUI() {
      document.title = t('title');
      document.getElementById('modeTitle').textContent = t('animationMode');
      document.getElementById('rainbowBtn').textContent = t('rainbow');
      document.getElementById('flashBtn').textContent = t('flash');
      document.getElementById('pulseBtn').textContent = t('pulse');
      document.getElementById('brightnessLabel').textContent = t('brightness');
      document.getElementById('autoModeTitle').textContent = t('autoMode');
      document.getElementById('vibrationLabel').textContent = t('vibrationTrigger');
      document.getElementById('clearBtn').textContent = t('clearLEDs');
      document.getElementById('statusTitle').textContent = t('status');
      document.getElementById('statusConnected').textContent = t('connected');
      
      var modes = [t('rainbowCycle'), t('randomFlash'), t('colorPulse')];
      document.getElementById('modeStatus').textContent = t('currentMode') + modes[currentMode];
      
      var langBtns = document.querySelectorAll('.lang-btn');
      for (var i = 0; i < langBtns.length; i++) {
        langBtns[i].classList.remove('active');
      }
      if (currentLang === 'en') langBtns[0].classList.add('active');
      else if (currentLang === 'zh-TW') langBtns[1].classList.add('active');
      else if (currentLang === 'zh-CN') langBtns[2].classList.add('active');
    }
    
    // Detect browser language and update
    function detectLanguage() {
      var browserLang = navigator.language || navigator.userLanguage;
      if (browserLang.indexOf('zh-Hans') !== -1 || browserLang === 'zh-CN') {
        currentLang = 'zh-CN';
      } else if (browserLang.indexOf('zh') !== -1) {
        currentLang = 'zh-TW';
      } else {
        currentLang = 'en';
      }
    }
    
    // Change language
    function setLanguage(lang) {
      currentLang = lang;
      updateUI();
    }
    
    function setMode(mode) {
      currentMode = mode;
      fetch('/api/setMode?mode=' + mode)
        .then(function(r) { return r.json(); })
        .then(function(data) {
          var modes = [t('rainbowCycle'), t('randomFlash'), t('colorPulse')];
          document.getElementById('modeStatus').textContent = t('currentMode') + modes[mode];
          var buttons = document.querySelectorAll('.mode-btn');
          for (var i = 0; i < buttons.length; i++) {
            buttons[i].classList.remove('active');
          }
          buttons[mode].classList.add('active');
        });
    }
    
    function updateBrightness() {
      var val = document.getElementById('brightness').value;
      document.getElementById('brightnessValue').textContent = val;
      fetch('/api/setBrightness?value=' + val)
        .then(function(r) { return r.json(); })
        .catch(function(e) { console.log(e); });
    }
    
    function toggleAutoMode() {
      fetch('/api/toggleAuto')
        .then(function(r) { return r.json(); })
        .then(function() {
          document.getElementById('autoToggle').classList.toggle('active');
        });
    }
    
    function clearAll() {
      fetch('/api/clear')
        .then(function(r) { return r.json(); })
        .then(function(data) { alert(t('cleared')); });
    }
    
    // Initialize on page load
    window.onload = function() {
      detectLanguage();
      updateUI();
    };
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n========== 互動玩具初始化 ==========");
  
  // LED初始化
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  FastLED.setDither(BINARY_DITHER);
  clearLEDs();
  FastLED.show();
  
  // 震動感應器初始化
  pinMode(VIBRATION_PIN, INPUT);
  
  // WiFi初始化
  initWiFi();
  
  // Web服務器路由
  server.on("/", handleRoot);
  server.on("/api/status", handleAPI);
  server.on("/api/setMode", handleSetMode);
  server.on("/api/setBrightness", handleSetColor);
  server.on("/api/clear", handleToggleAuto);
  server.on("/api/toggleAuto", handleToggleAuto);
  server.begin();
  
  Serial.println("🚀 Web服務器已啟動");
  Serial.print("📱 訪問: http://");
  Serial.print(WiFi.softAPIP());
  Serial.println("/");
  Serial.println("===================================\n");
}

void loop() {
  // 處理Web請求
  server.handleClient();
  
  // 檢測震動
  if (autoMode && digitalRead(VIBRATION_PIN) == HIGH) {
    unsigned long currentTime = millis();
    if (currentTime - lastVibrationTime > vibrationCooldown) {
      handleVibration();
      lastVibrationTime = currentTime;
    }
  }
  
  // 更新動畫
  updateAnimation();
  
  FastLED.show();
  delay(30);
}

void initWiFi() {
  Serial.print(F("Setting WiFi AP..."));
  WiFi.mode(WIFI_AP); WiFi.setSleep(false);
  uint8_t macAddr[6];
  WiFi.softAPmacAddress(macAddr);
    while (!WiFi.softAP(String(TOY_SSID)+String(macAddr[3]+macAddr[4]+macAddr[5],HEX), TOY_PWD, 1, false, 1)) {
    Serial.print(F("."));
  }
  Serial.print(F("successfully!"));
  Serial.println("SSID=" + String(TOY_SSID) + String(macAddr[3]+macAddr[4]+macAddr[5],HEX));
}

void handleRoot() {
  server.send(200, "text/html; charset=utf-8", htmlPage);
}

void handleAPI() {
  String response = "{\"status\":\"ok\",\"mode\":" + String(animationMode) + "}";
  server.send(200, "application/json", response);
}

void handleSetMode() {
  if (server.hasArg("mode")) {
    int mode = server.arg("mode").toInt();
    setAnimationMode(mode);
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"缺少參數\"}" );
  }
}

void handleSetColor() {
  if (server.hasArg("value")) {
    int brightness = server.arg("value").toInt();
    FastLED.setBrightness(brightness);
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  }
}

void handleToggleAuto() {
  if (server.uri() == "/api/toggleAuto") {
    autoMode = !autoMode;
    Serial.print("🔄 自動模式: ");
    Serial.println(autoMode ? "啟用" : "禁用");
  } else if (server.uri() == "/api/clear") {
    clearLEDs();
    FastLED.show();
    Serial.println("🟢 LED已清空");
  }
  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void setAnimationMode(int mode) {
  animationMode = mode;
  brightnesLevel = 255;
  animationTimer = millis();
  Serial.print("📺 模式切換: ");
  
  switch(mode) {
    case 0: Serial.println("彩虹循環"); break;
    case 1: Serial.println("隨機閃爍"); break;
    case 2: Serial.println("顏色脈衝"); break;
  }
}

void handleVibration() {
  Serial.println("✨ 偵測到震動！");
  animationMode = (animationMode + 1) % 3;
  brightnesLevel = 255;
  animationTimer = millis();
}

void updateAnimation() {
  unsigned long elapsed = millis() - animationTimer;
  int calcValue = (int)brightnesLevel - (elapsed / 20);
  uint8_t brightness = (calcValue > 0) ? (uint8_t)calcValue : 0;
  brightnesLevel = brightness;
  
  if (animationMode == 0) {
    rainbowCycle(brightness);
  } else if (animationMode == 1) {
    randomFlash();
  } else if (animationMode == 2) {
    colorPulse(CRGB::Cyan, brightness);
  }
}

void rainbowCycle(uint8_t brightness) {
  static uint8_t hue = 0;
  hue += 3;
  
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((hue + i * 255 / NUM_LEDS), 255, brightness);
  }
}

void randomFlash() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(random(255), random(255), random(255));
  }
}

void colorPulse(CRGB color, uint8_t brightness) {
  static uint8_t pulseValue = 0;
  pulseValue += 5;
  
  uint8_t fade = sin8(pulseValue);
  uint8_t finalBrightness = (brightness * fade) / 255;
  
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
    leds[i].nscale8(finalBrightness);
  }
}

void clearLEDs() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
}