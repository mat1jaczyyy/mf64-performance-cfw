def convert_xy_mf(xy: int):
    x = xy % 10
    y = xy // 10

    x = x - 1 # 0-based index
    y = y - 1 # 0-based index

    return x + (y * 8)

for i in range(100):
    layout.append(convert_xy_mf(i))