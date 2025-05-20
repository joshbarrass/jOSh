color_map = {
    "r": "VGA_COLOR_RED",
    "w": "VGA_COLOR_WHITE",
    "Y": "VGA_COLOR_LIGHT_GREY",
}

with open("panic_image") as f:
    data = f.read()

chars, colors = data.split("\n\n")
chars = chars.split("\n")
colors = colors.split("\n")
line_length = len(chars[0])
for i, line in enumerate(chars[1:]):
    if len(line) == 0:
        continue
    try:
        assert len(line) == line_length
    except AssertionError:
        print(
            f"Line {i+1} wrong length ({len(line)} != {line_length})"
        )
        raise
for i, line in enumerate(colors):
    if len(line) == 0:
        continue
    try:
        assert len(line) == 2 * line_length
    except AssertionError:
        print(f"Line {i} wrong length ({len(line)} != {line_length})")
        raise
chars = "".join(chars)
colors = "".join(colors)

print("{")
for i, char in enumerate(chars):
    if char == '0':
        char = '\0'
    if char == "\\":
        char = "\\\\"
    print(
        f"{{{char}, {{{color_map[colors[2*i]]}, {color_map[colors[2*i+1]]}}}}},"
    )
print("};")
