#!/usr/bin/env python3
import serial, sys, tty, termios, time

PORT = '/dev/cu.usbserial-1140'
BAUD = 115200

print("=" * 60)
print(" ⏱️ ESP32-S3 50분 타이머 & GIF 표정 맥 키보드 컨트롤러")
print("=" * 60)
print(" [ 스페이스바 / S ] : 타이머 시작 / 일시정지 (TOGGLE)")
print(" [ R ]             : 50분 리셋 (Reset)")
print(" [ 1 ]             : Default GIF (기본 로봇 눈)")
print(" [ 2 ]             : Surprise GIF (놀란 눈 😯)")
print(" [ 3 ]             : Crying GIF (우는 눈 😭)")
print(" [ M ]             : 하단 안내 문구 직접 입력")
print(" [ Q ]             : 컨트롤러 종료")
print("=" * 60)

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

def send_cmd(cmd):
    ser.write((cmd + '\n').encode('utf-8'))
    print(f"\r[보낸 신호]: {cmd}                     \n> ", end='', flush=True)

try:
    while True:
        print("> ", end='', flush=True)
        ch = getch()
        if ch == ' ' or ch.lower() == 's':
            send_cmd("TOGGLE")
        elif ch.lower() == 'r':
            send_cmd("RESET")
        elif ch == '1':
            send_cmd("EXPR:0")
            print(f"\r[표정 강제 변경]: Default GIF              ")
        elif ch == '2':
            send_cmd("EXPR:1")
            print(f"\r[표정 강제 변경]: Surprise GIF 😯          ")
        elif ch == '3':
            send_cmd("EXPR:2")
            print(f"\r[표정 강제 변경]: Crying GIF 😭            ")
        elif ch.lower() == 'm':
            termios.tcsetattr(sys.stdin.fileno(), termios.TCSADRAIN, termios.tcgetattr(sys.stdin.fileno()))
            sys.stdout.write("\r새로운 안내 문구 입력 (영문): ")
            sys.stdout.flush()
            text = sys.stdin.readline().strip()
            if text:
                send_cmd(f"MSG:{text}")
        elif ch.lower() == 'q':
            print("\r\n👋 컨트롤러를 종료합니다.")
            break
except KeyboardInterrupt:
    print("\r\n👋 컨트롤러를 종료합니다.")
finally:
    ser.close()
