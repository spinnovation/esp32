#!/usr/bin/env python3
import math
from PIL import Image, ImageDraw

w, h = 300, 92
bg_color = (11, 13, 20) # Midnight Dark #0B0D14
cyan_color = (0, 229, 255) # Neon Cyan
gold_color = (255, 215, 0) # Gold Happy
pink_color = (255, 51, 153) # Cute Pink

eye_w, eye_h = 52, 52
spacing = 90
center_x, center_y = w // 2, h // 2

frames = []
durations = []

total_frames = 80

for f in range(total_frames):
    im = Image.new('RGB', (w, h), bg_color)
    draw = ImageDraw.Draw(im)

    curr_color = cyan_color
    offset_x = 0
    offset_y = 0
    l_h = eye_h
    r_h = eye_h
    l_w = eye_w
    r_w = eye_w

    if f < 20: # 1. Wide Gaze Left & Right + Blink
        progress = f / 20.0
        offset_x = 65 * math.sin(progress * 2 * math.pi)
        if f in [5, 6, 15, 16]:
            l_h, r_h = 8, 8

    elif f < 35: # 2. Playful Wink & Happy Smile (^ ^)
        curr_color = gold_color
        if f < 27:
            l_h = 8
            r_h = 44
        else:
            l_h, r_h = 44, 44
        offset_y = -3 if f % 2 == 0 else 3

    elif f < 50: # 3. Playful Teardrop & Sad Droop (T T)
        curr_color = (255, 51, 102)
        l_h, r_h = 16, 16
        drop_y = center_y + 10 + ((f * 4) % 25)
        draw.ellipse([center_x + 65, drop_y, center_x + 75, drop_y + 14], fill=cyan_color)

    elif f < 65: # 4. Curious / Wonder Uneven Eyes (O o)
        curr_color = pink_color
        l_w, l_h = 58, 58
        r_w, r_h = 36, 36
        offset_y = -6

    else: # 5. Return to Normal Center & Wink Goodbye
        curr_color = cyan_color
        if f in [70, 71]:
            r_h = 8

    lx = center_x - (spacing // 2) + int(offset_x)
    rx = center_x + (spacing // 2) + int(offset_x)
    cy = center_y + int(offset_y)

    l_box = [lx - l_w//2, cy - l_h//2, lx + l_w//2, cy + l_h//2]
    draw.rounded_rectangle(l_box, radius=min(24, l_h//2), fill=curr_color)

    r_box = [rx - r_w//2, cy - r_h//2, rx + r_w//2, cy + r_h//2]
    draw.rounded_rectangle(r_box, radius=min(24, r_h//2), fill=curr_color)

    if 27 <= f < 35:
        draw.rectangle([lx - l_w//2 - 2, cy + 4, lx + l_w//2 + 2, cy + l_h//2 + 2], fill=bg_color)
        draw.rectangle([rx - r_w//2 - 2, cy + 4, rx + r_w//2 + 2, cy + r_h//2 + 2], fill=bg_color)

    frames.append(im.convert('P', palette=Image.Palette.ADAPTIVE))
    durations.append(200) # 200ms per frame = 0.5x speed (Slower, calm, smooth)

out_gif_path = '/Users/onkistudio/내 드라이브/software/esp32/expression/default.gif'
frames[0].save(
    out_gif_path,
    save_all=True,
    append_images=frames[1:],
    optimize=True,
    duration=durations,
    loop=0
)

print(f"Successfully generated 0.5x slower default.gif ({len(frames)} frames)!")
