<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="funxedu-logo4-white.png">
    <source media="(prefers-color-scheme: light)" srcset="funxedu-logo4-black.png">
    <img alt="funXedu" src="funxedu-logo4-black.png" style="max-width: 100%;">
  </picture>
  <br/>
</p>

# funXtoy - 互動LED玩具控制系統

<p align="center">
  <strong>智慧化互動玩具 | 多彩LED動畫 | 震動感應控制 | 網頁遠端操控</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Platform-ESP8266-blue" alt="Platform">
  <img src="https://img.shields.io/badge/Framework-Arduino-brightgreen" alt="Framework">
  <img src="https://img.shields.io/badge/License-MIT-green" alt="License">
  <img src="https://img.shields.io/badge/Language-C%2B%2B-orange" alt="Language">
</p>

---

## 📋 目錄

- [功能特點](#功能特點)
- [硬體需求](#硬體需求)
- [軟體要求](#軟體要求)
- [快速開始](#快速開始)
- [安裝指南](#安裝指南)
- [使用說明](#使用說明)
- [API 文檔](#api-文檔)
- [動畫模式](#動畫模式)
- [故障排除](#故障排除)

---

## 🎯 功能特點

### 🌈 多彩動畫效果
- **彩虹循環** - 平滑的彩虹色彩漸變循環
- **隨機閃爍** - 隨機RGB顏色的動態閃爍
- **顏色脈衝** - 可自訂顏色的柔和脈衝效果
- **跑馬燈** - 持續移動的光點效果，附帶尾跡淡出

### 📱 智慧控制
- **WiFi 無線控制** - 透過手機/電腦網頁遠端控制
- **震動感應** - 實體震動感應器自動觸發動畫切換
- **自動模式** - 支援自動和手動雙模式
- **亮度調節** - 0-255等級的無段亮度控制

### 🎨 顏色自訂
- **色彩選擇器** - 直觀的網頁色彩選擇介面
- **RGB 輸入** - 精確的 R/G/B 數值設定
- **十六進制支援** - 支援 #RRGGBB 格式

### 🌍 多語言支援
- **英語** English
- **繁體中文** 繁體中文
- **簡體中文** 简体中文
- 自動語言檢測 + 手動選擇

### 📊 實時回饋
- 動畫模式實時顯示
- 亮度數值即時反映
- WiFi 連線狀態監控
- Serial 控制台詳細日誌

---

## 🔧 硬體需求

### 微控制器
| 項目 | 規格 |
|------|------|
| **主板** | ESP01S (ESP8266) |
| **GPIO** | GPIO0, GPIO2 (可用) |
| **工作電壓** | 3.3V |
| **Flash** | 1MB |

### 電子元件
| 元件 | 規格 | 數量 |
|------|------|------|
| **LED 燈帶** | WS2812B (5V) | 8個LED |
| **震動感應器** | 震動開關傳感器 | 1個 |
| **USB 電源** | 5V 1A 以上 | 1個 |
| **Micro USB 線** | 程序下載 | 1個 |

### 連線圖
```
ESP01S GPIO0 → 震動感應器
ESP01S GPIO2 → WS2812B 燈帶 (資料線, 通過470Ω電阻)
ESP01S 3.3V  → 震動感應器 (VCC)
ESP01S GND   → 震動感應器 + 燈帶 (GND)
5V 電源      → WS2812B 燈帶 (VCC)
```

---

## 💻 軟體要求

### 開發環境
- **IDE**: Visual Studio Code
- **平台**: PlatformIO for VSCode
- **框架**: Arduino Framework
- **Python**: 3.8+ (用於本地測試)

### 必需庫
```ini
lib_deps = 
    fastled/FastLED@^3.10.3
    ESP8266WiFi @ 1.0
    ESP8266WebServer @ 1.0
```

---

## 🚀 快速開始

### 1️⃣ 安裝 VSCode 和 PlatformIO
```bash
pip install platformio
```

### 2️⃣ 克隆專案
```bash
git clone https://github.com/robot4fun/funXtoy1.git
cd funXtoy1
```

### 3️⃣ 編譯和上傳
```bash
# 編譯
platformio run

# 上傳到 ESP01S
platformio run -t upload

# 監控 Serial 輸出
platformio device monitor --baud 115200
```

### 4️⃣ 連接 WiFi
1. 設備上電後，自動啟動 WiFi AP 模式
2. 搜尋 WiFi 網絡：`funXtoy_XXXXXX`
3. 密碼：`123456`
4. 在瀏覽器中訪問：`http://192.168.4.1/`

---

## 📖 完整安裝指南

### 硬體組裝

#### 1. 震動感應器
```
VCC → ESP01S 3.3V
GND → ESP01S GND
SIG → ESP01S GPIO0
```

#### 2. WS2812B LED 燈帶
```
DIN → ESP01S GPIO2 (通過 470Ω 電阻)
+5V → 5V 電源
GND → GND (共地)
```

#### 3. 軟體安裝
```bash
# 1. 編譯
platformio run

# 2. 連接 ESP01S
# (按住 IO0 按鈕進入燒錄模式)

# 3. 上傳
platformio run -t upload

# 4. 驗證
platformio device monitor --baud 115200
```

預期输出：
```
🚀 Web服務器已啟動
📱 訪問: http://192.168.4.1/
===================================
```

---

## 📱 使用說明

### 網頁介面

#### 動畫模式
- 🌈 **彩虹** - Rainbow Cycle
- ⚡ **閃爍** - Random Flash
- 💫 **脈衝** - Color Pulse (可選色)
- 🏃 **跑馬燈** - Chase (可選色)

#### 控制功能
- **亮度滑塊** - 0-255 級調節
- **色彩選擇器** - RGB / 十六進制輸入
- **自動模式** - 震動感應開關
- **清空 LED** - 熄滅所有燈

#### 語言選擇
支援英文、繁體中文、簡體中文 (自動檢測)

### 實體操作
- 輕輕震動設備即可切換動畫 (需啟用自動模式)

---

## 🔌 API 文檔

所有 API 端點: `http://192.168.4.1/api/[端點]`

### 設置動畫模式
```
GET /api/setMode?mode=[0|1|2|3]

mode=0: 彩虹循環
mode=1: 隨機閃爍
mode=2: 顏色脈衝
mode=3: 跑馬燈

範例: http://192.168.4.1/api/setMode?mode=2
```

### 設置亮度
```
GET /api/setBrightness?value=[0-255]

範例: http://192.168.4.1/api/setBrightness?value=200
```

### 設置顏色
```
GET /api/setColor?r=[0-255]&g=[0-255]&b=[0-255]

範例: http://192.168.4.1/api/setColor?r=255&g=0&b=255
```

### 清空 LED
```
GET /api/clear

範例: http://192.168.4.1/api/clear
```

### 切換自動模式
```
GET /api/toggleAuto

範例: http://192.168.4.1/api/toggleAuto
```

---

## 🎨 動畫模式詳解

### 1. 彩虹循環 (Rainbow Cycle)
- 平滑漸變的彩虹色彩
- 連續循環播放
- 用途: 靜止展示、氛圍營造

### 2. 隨機閃爍 (Random Flash)
- 每個 LED 隨機顏色快速變化
- 高頻率更新 (~30fps)
- 用途: 活潑效果、派對模式

### 3. 顏色脈衝 (Color Pulse)
- 單一可選顏色，正弦波亮度脈衝
- 2 秒後逐漸淡出
- 用途: 節奏提示、通知效果

### 4. 跑馬燈 (Chase)
- 光點持續移動，帶尾跡淡出
- 100ms 移動一個 LED
- 用途: 動態展示、引導視線

---

## 🐛 故障排除

### LED 不亮
- 檢查 GPIO2 接線
- 驗證 5V 電源連接
- 確認 GND 共接

### 無法連接 WiFi
- 確認 ESP01S 已上電
- 檢查 USB 線連接
- 重新上載代碼

### 震動感應無反應
- 檢查 GPIO0 接線
- 驗證震動感應器規格
- 執行: `curl http://192.168.4.1/api/toggleAuto`

### 調試技巧
```bash
# 啟動 Serial 監控
platformio device monitor --baud 115200

# 測試 API
curl "http://192.168.4.1/api/setColor?r=255&g=0&b=0"
```

---

## 📊 文件結構

```
funXtoy1/
├── src/main.cpp              # 主程序
├── platformio.ini            # 配置文件
├── preview.html              # 獨立測試頁面
└── README.md                 # 本文檔
```

---

## 📄 MIT License

Copyright (c) 2026 funXedu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction...

---

**版本**: 1.0.0  
**更新日期**: 2026年2月22日  
**狀態**: ✅ 生產就緒