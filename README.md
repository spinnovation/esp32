# ESP32-S3 4.0" (480x480) LVGL 50분 카운트다운 타이머 & 디지털 시계

ESP32-S3 4.0인치 480x480 IPS 디스플레이(ST7701S + GT911 정전식 터치) 보드를 위한 **LVGL 기반 50분 스마트 카운트다운 타이머 & 디지털 시계** 프로젝트입니다.

---

## 📸 주요 기능 (Features)

1. **디폴트 50분 카운트다운 타이머 (50-Min Focus Timer)**
   - 50:00 분 기본 설정 (공부, 몰입 작업에 최적화).
   - 네온 스타일 360도 원형 progress arc 애니메이션.
   - 원터치 퀵 프리셋 버튼 (`50m`, `25m`, `15m`, `10m`, `5m`).
   - 터치 기반 `[ ▶ START / ❚❚ PAUSE ]`, `[ ↺ RESET ]` 버튼.

2. **상단 디지털 시계 (Digital Clock)**
   - 화면 상단 헤더에 실시간 디지털 시계 표시 (`HH:MM:SS AM/PM`).
   - Wi-Fi 연결 시 한국 표준시(KST, UTC+9) NTP 시간 자동 동기화.
   - Wi-Fi 미연결 시 시스템 업타임 시계 자동 전환.

3. **알람 및 톤 피드백**
   - 타이머 00:00 도달 시 화면 빨간색 경고 표시 및 PWM 부저 알람음 출력.

---

## 📂 프로젝트 구조

```text
/Users/onkistudio/software/esp32/
├── platformio.ini         # PlatformIO 환경 설정 (ESP32-S3 PSRAM, LVGL)
├── include/
│   ├── config.h           # LCD / 터치 / 부저 GPIO 핀배치 및 디폴트 50분 설정
│   ├── display_driver.h   # ST7701S RGB 디스플레이 드라이버 헤더
│   ├── touch_driver.h     # GT911 Capacitive Touch 드라이버 헤더
│   ├── timer_logic.h      # 50분 타이머 FSM 로직 클래스
│   ├── ui_timer.h         # LVGL 8 UI 뷰 헤더
│   └── lv_conf.h          # LVGL 8 세부 기능 설정
├── src/
│   ├── display_driver.cpp # ST7701S 480x480 RGB 패널 초기화 & PSRAM 버퍼링
│   ├── touch_driver.cpp   # GT911 I2C 터치 입력 맵핑
│   ├── timer_logic.cpp    # 카운트다운 및 부저 알람 제어
│   ├── ui_timer.cpp       # LVGL UI 화면 구성 (Arc, Label, Button, Colors)
│   └── main.cpp           # Arduino setup(), loop() 및 Wi-Fi NTP 시간 설정
└── README.md
```

---

## 🛠️ 빌드 및 업로드 방법 (Build & Upload Guide)

### 방법 1: PlatformIO (추천)
VS Code의 PlatformIO IDE 확장이 설치되어 있다면:
```bash
# 프로젝트 폴더 이동
cd /Users/onkistudio/software/esp32

# 빌드 실행
pio run

# 보드 연결 후 업로드 및 시리얼 모니터
pio run -t upload -t monitor
```

### 방법 2: Arduino IDE
1. Arduino IDE에서 **ESP32 보드 매니저** 설치 (`esp32` by Espressif).
2. **필수 라이브러리 설치**:
   - `lvgl` (v8.3.11)
   - `Arduino_GFX_Library` (v1.4.0 이상)
   - `TAMC_GT911`
3. 보드 설정:
   - Board: **ESP32S3 Dev Module**
   - PSRAM: **OPI PSRAM** (8MB)
   - Flash Size: **16MB (128Mb)**
4. `src/` 내 모든 `.cpp` 및 `.h` 파일, `main.cpp`를 프로젝트 폴더로 로드 후 **Upload**.

---

## 📶 Wi-Fi NTP 시간 설정 (선택 사항)

`src/main.cpp` 파일 상단의 SSID와 Password를 수정하면 정확한 한국 표준시가 자동으로 동기화됩니다.

```cpp
const char* WIFI_SSID = "YOUR_WIFI_SSID";     // 사용 중인 와이파이 이름
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD"; // 와이파이 비밀번호
```

---

## 📌 주요 핀맵 참고 (Pinout: ESP32-4848S040)

- **Display (ST7701S RGB 16-bit)**: DE: `GPIO 40`, VSYNC: `GPIO 41`, HSYNC: `GPIO 42`, PCLK: `GPIO 39`
- **Touch (GT911 I2C)**: SDA: `GPIO 19`, SCL: `GPIO 20`, INT: `GPIO 18`, RST: `GPIO 38`
- **Backlight**: `GPIO 38`
- **Buzzer**: `GPIO 4`
