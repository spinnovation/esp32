#!/usr/bin/env python3
import os
from PIL import Image, ImageSequence, ImageOps

exp_dir = '/Users/onkistudio/software/esp32/expression'
out_header = '/Users/onkistudio/software/esp32/include/gif_data.h'

target_w, target_h = 240, 100

gif_files = [f for f in sorted(os.listdir(exp_dir)) if f.endswith('.gif')]
print(f"Found {len(gif_files)} GIF files: {gif_files}")

gif_arrays = []

for idx, fname in enumerate(gif_files):
    fpath = os.path.join(exp_dir, fname)
    im = Image.open(fpath)

    resized_frames = []
    durations = []

    for frame in ImageSequence.Iterator(im):
        f = frame.convert('RGBA')
        # Scale to fit inside target_w x target_h
        f.thumbnail((target_w, target_h), Image.Resampling.LANCZOS)
        # Create black background image
        bg = Image.new('RGBA', (target_w, target_h), (11, 13, 20, 255)) # Midnight dark background
        paste_x = (target_w - f.width) // 2
        paste_y = (target_h - f.height) // 2
        bg.paste(f, (paste_x, paste_y), f)
        resized_frames.append(bg.convert('P', palette=Image.Palette.ADAPTIVE))
        durations.append(frame.info.get('duration', 80))

    out_gif_path = f"/tmp/resized_{idx}.gif"
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
        var_name = f"gif_data_{idx}"
        gif_arrays.append((var_name, len(data), data, fname))
        print(f"Processed {fname}: {len(data)} bytes")

# Write to header file
with open(out_header, 'w') as f:
    f.write("#ifndef GIF_DATA_H\n#define GIF_DATA_H\n\n#include <pgmspace.h>\n#include <stdint.h>\n\n")
    for var_name, size, data, fname in gif_arrays:
        f.write(f"// Source: {fname}\n")
        f.write(f"const uint8_t {var_name}[{size}] PROGMEM = {{\n")
        for i in range(0, size, 16):
            chunk = data[i:i+16]
            hex_str = ", ".join(f"0x{b:02X}" for b in chunk)
            f.write(f"  {hex_str},\n")
        f.write("};\n\n")

    f.write(f"#define NUM_GIFS {len(gif_arrays)}\n\n")
    f.write("struct GifItem {\n  const uint8_t* data;\n  size_t size;\n  const char* name;\n};\n\n")
    f.write("const GifItem GIF_LIST[NUM_GIFS] = {\n")
    for var_name, size, data, fname in gif_arrays:
        safe_name = fname.replace('"', '\\"')
        f.write(f"  {{ {var_name}, {size}, \"{safe_name}\" }},\n")
    f.write("};\n\n#endif // GIF_DATA_H\n")

print("Successfully generated src/gif_data.h!")
