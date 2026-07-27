#!/usr/bin/env python3
import os
from PIL import Image, ImageSequence

base_dir = os.path.dirname(os.path.abspath(__file__))
exp_dir = os.path.join(base_dir, 'expression')
out_header = os.path.join(base_dir, 'include', 'gif_data.h')

target_w, target_h = 300, 92  # Fills ~85% of top section height

gifs = {
    'default': 'default.gif',
    'surprise': 'surprise.gif',
    'crying': 'crying.gif'
}

gif_results = {}

for key, fname in gifs.items():
    fpath = os.path.join(exp_dir, fname)
    if not os.path.exists(fpath):
        print(f"ERROR: {fname} missing at {fpath}")
        continue

    im = Image.open(fpath)
    resized_frames = []
    durations = []

    for frame in ImageSequence.Iterator(im):
        f = frame.convert('RGBA')
        f.thumbnail((target_w, target_h), Image.Resampling.LANCZOS)

        bg = Image.new('RGBA', (target_w, target_h), (11, 13, 20, 255)) # Midnight dark background
        paste_x = (target_w - f.width) // 2
        paste_y = (target_h - f.height) // 2
        bg.paste(f, (paste_x, paste_y), f)

        resized_frames.append(bg.convert('P', palette=Image.Palette.ADAPTIVE))
        durations.append(frame.info.get('duration', 80))

    out_gif_path = f"/tmp/resized_{key}.gif"
    if resized_frames:
        resized_frames[0].save(
            out_gif_path,
            save_all=True,
            append_images=resized_frames[1:],
            optimize=True,
            duration=durations,
            loop=0
        )
        with open(out_gif_path, 'rb') as f:
            data = f.read()
        var_name = f"gif_data_{key}"
        gif_results[key] = (var_name, len(data), data)
        print(f"Processed {key} ({fname}): {len(data)} bytes")

# Write to gif_data.h
with open(out_header, 'w') as f:
    f.write("#ifndef GIF_DATA_H\n#define GIF_DATA_H\n\n#include <pgmspace.h>\n#include <stdint.h>\n\n")

    for key, (var_name, size, data) in gif_results.items():
        f.write(f"// GIF State: {key}\n")
        f.write(f"const uint8_t {var_name}[{size}] PROGMEM = {{\n")
        for i in range(0, size, 16):
            chunk = data[i:i+16]
            hex_str = ", ".join(f"0x{b:02X}" for b in chunk)
            f.write(f"  {hex_str},\n")
        f.write("};\n\n")

    f.write(f"#define GIF_DEFAULT_SIZE {gif_results['default'][1]}\n")
    f.write(f"#define GIF_SURPRISE_SIZE {gif_results['surprise'][1]}\n")
    f.write(f"#define GIF_CRYING_SIZE {gif_results['crying'][1]}\n\n")
    f.write("#endif // GIF_DATA_H\n")

print("Successfully generated include/gif_data.h!")
