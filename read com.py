import struct
import csv
import time
import serial.tools.list_ports

# ===== 保留你的串口连接逻辑 =====
PORTS = ['COM10', 'COM11', 'COM16']
BAUD = 921600

ser = None
print("等待串口连接...")

while ser is None:
    available_ports = [p.device for p in serial.tools.list_ports.comports()]
    for p in PORTS:
        if p in available_ports:
            try:
                ser = serial.Serial(p, BAUD)
                print(f"串口已连接: {p}")
                break
            except Exception as e:
                print(f"打开串口 {p} 失败: {e}")
    if ser is None:
        time.sleep(1)
# =================================
ser.reset_input_buffer()

# ================== CSV文件 ==================
filename = time.strftime("adc_%Y%m%d_%H%M%S.csv")
f = open(filename, 'w', newline='')
writer = csv.writer(f)
writer.writerow(["index","ch1","ch2","ch3","ch4"])
f.flush()
# ============================================

buffer = bytearray()
SAMPLE_SIZE = 16  # 4B head + 4B index + 4*2B channels
frame_head_bytes = b'\x55\xAA\x55\xAA'

last_index = None
lost_count = 0

while True:
    buffer += ser.read(4096)

    while True:
        pos = buffer.find(frame_head_bytes)
        if pos < 0:
            # 帧头没找到，清空缓存
            buffer.clear()
            break

        if len(buffer) < pos + SAMPLE_SIZE:
            break

        frame = buffer[pos:pos+SAMPLE_SIZE]
        buffer = buffer[pos+SAMPLE_SIZE:]

        # 解析
        head = struct.unpack('<I', frame[0:4])[0]
        index = struct.unpack('<I', frame[4:8])[0]
        ch = struct.unpack('<4H', frame[8:16])

        # 丢包检测
        if last_index is not None:
            if index != last_index + 1:
                lost = index - last_index - 1
                lost_count += lost
                print(f"❌ 丢包: {last_index} -> {index}  丢了 {lost}")
        last_index = index

        print(index, *ch)
        writer.writerow([index, *ch])
        f.flush()
