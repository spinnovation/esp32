#!/usr/bin/env python3
import serial, sys, tty, termios, time

PORT = '/dev/cu.usbserial-1140'
BAUD = 115200

print("=" * 55)
print(" ⏱️ ESP32-S3 50분 타이머 맥 키보드 컨트롤러")
print("=" * 55)
print(" [ 스페이스바 / S ] : 타이머 시작 / 일시정지")
print(" [ R ]             : 50분 리셋 (Reset)")
print(" [ M ]             : 하단 안내 문구 직접 입력")
print(" [ Q ]             : 컨트롤러 종료")
print("=" * 55)

try:
    ser = serial.Serial(PORT, BAUD, timeout=0.5)
    time.sleep(1.0)
except Exception as e:
    print(f"❌ 포트 연결 실패: {e}")
    sys.exit(1)

def getch():
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(sys.stdin.fileno())
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    return ch

try:
    is_running = False
    while True:
        char = getch()
        if char == ' ' or char.lower() == 's':
            if not is_running:
                ser.write(b'START\n')
                print("\r▶️  타이머 시작 (50:00)              ", end="", flush=True)
                is_running = True
            else:
                ser.write(b'PAUSE\n')
                print("\r⏸️  타이머 일시정지                 ", end="", flush=True)
                is_running = False
        elif char.lower() == 'r':
            ser.write(b'RESET\n')
            print("\r🔄  타이머 리셋 (50:00)             ", end="", flush=True)
            is_running = False
        elif char.lower() == 'm':
            print("\r\n💬 새로운 안내 문구를 입력하세요: ", end="", flush=True)
            msg = input()
            if msg.strip():
                ser.write(f"MSG:{msg.strip()}\n".encode('utf-8'))
                print(f"✅ 문구 변경 완료: {msg.strip()}")
        elif char.lower() == 'q' or ord(char) == 3:
            print("\r\n👋 컨트롤러를 종료합니다.")
            break
except KeyboardInterrupt:
    print("\r\n👋 종료되었습니다.")
finally:
    ser.close()
