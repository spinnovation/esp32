#!/usr/bin/env python3
import math
from PIL import Image, ImageDraw

w, h = 300, 92
bg_color = (11, 13, 20) # Midnight Dark #0B0D14
eye_color = (0, 229, 255) # Neon Cyan #00E5FF

eye_w, eye_h = 52, 52
spacing = 90 # Distance between left and right eye centers
center_x, center_y = w // 2, h // 2

frames = []
durations = []

# Total trajectory frames (~40 frames @ 100ms = 4.0 seconds total loop)
total_frames = 40

for f in range(total_frames):
    t = f / total_frames # 0.0 to 1.0

    # Sine / Cosine smooth gaze movement with wider range (-65 to +65)
    # 0.0 to 0.2: Center
    # 0.2 to 0.4: Move Far Left (-65)
    # 0.4 to 0.6: Hold Left
    # 0.6 to 0.8: Move Far Right (+65)
    # 0.8 to 1.0: Move back to Center

    if t < 0.15:
        offset_x = 0
    elif t < 0.35:
        progress = (t - 0.15) / 0.20
        offset_x = -65 * math.sin(progress * math.pi / 2)
    elif t < 0.50:
        offset_x = -65
    elif t < 0.75:
        progress = (t - 0.50) / 0.25
        offset_x = -65 + 130 * (0.5 - 0.5 * math.cos(progress * math.pi))
    elif t < 0.85:
        offset_x = 65
    else:
        progress = (t - 0.85) / 0.15
        offset_x = 65 * math.cos(progress * math.pi / 2)

    # Blink at start and near end
    curr_eye_h = eye_h
    if f in [2, 3] or f in [35, 36]:
        curr_eye_h = 8 # Squished blink

    im = Image.new('RGB', (w, h), bg_color)
    draw = ImageDraw.Draw(im)

    lx = center_x - (spacing // 2) + int(offset_x)
    rx = center_x + (spacing // 2) + int(offset_x)
    cy = center_y

    # Draw Left Eye
    l_box = [lx - eye_w//2, cy - curr_eye_h//2, lx + eye_w//2, cy + curr_eye_h//2]
    draw.rounded_rectangle(l_box, radius=min(24, curr_eye_h//2), fill=eye_color)

    # Draw Right Eye
    r_box = [rx - eye_w//2, cy - curr_eye_h//2, rx + eye_w//2, cy + curr_eye_h//2]
    draw.rounded_rectangle(r_box, radius=min(24, curr_eye_h//2), fill=eye_color)

    frames.append(im.convert('P', palette=Image.Palette.ADAPTIVE))
    durations.append(100) # Slower 100ms per frame

# Save to expression/default.gif
out_gif_path = '/Users/onkistudio/내 드라이브/software/esp32/expression/default.gif'
frames[0].save(
    out_gif_path,
    save_all=True,
    append_images=frames[1:],
    optimize=True,
    duration=durations,
    loop=0
)

print(f"Successfully generated new wider & slower default.gif ({len(frames)} frames)!")
