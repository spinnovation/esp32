#!/usr/bin/env python3
import os
from PIL import Image, ImageDraw, ImageFont

# Font file path on macOS
font_path = "/System/Library/Fonts/Supplemental/AppleGothic.ttf"
if not os.path.exists(font_path):
    font_path = "/System/Library/Fonts/AppleSDGothicNeo.ttc"

print(f"Using font: {font_path}")

# Characters to render in Korean
chars = "안녕하세요 승필님집중시간입니다수고하셨습니다휴식일시정지시작리셋월화수목금토일0123456789: -|/!🔥🎉⏸️▶️"

font = ImageFont.truetype(font_path, 18)

glyph_data = []

for ch in sorted(list(set(chars))):
    code = ord(ch)
    # Create image for character
    img = Image.new('L', (24, 24), 0)
    draw = ImageDraw.Draw(img)
    draw.text((2, 0), ch, font=font, fill=255)
    
    bbox = img.getbbox()
    if bbox:
        w = bbox[2] - bbox[0]
        h = bbox[3] - bbox[1]
        cropped = img.crop(bbox)
    else:
        w, h = 10, 18
        cropped = Image.new('L', (w, h), 0)

    # Convert 8-bit alpha to 1-bit or 4-bit bitmap data
    bytes_list = []
    for y in range(h):
        for x in range(w):
            val = cropped.getpixel((x, y))
            bytes_list.append(val)

    glyph_data.append({
        'code': code,
        'char': ch,
        'w': w,
        'h': h,
        'bytes': bytes_list
    })

print(f"Generated {len(glyph_data)} glyphs.")
