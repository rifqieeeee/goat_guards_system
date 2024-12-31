# -*- coding: utf-8 -*-
import time
from pinpong.board import Board, UART

Board("UNIHIKER").begin()  # Initialize the UNIHIKER board. Select the board type, if not specified, it will be automatically detected.

#hardware UART 1 : P0-RX    P3-TX
uart1 = UART(bus_num=0)   # Initialize UART (Hardware Serial 1)

# Initialize UART: `baud_rate` for baud rate, `bits` for the number of data bits (8/9), `parity` for parity check (0 none/1 odd/2 even), `stop` for stop bits (1/2).
uart1.init(baud_rate=9600, bits=8, parity=0, stop=1)  # Kode1

buf = [0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07]

while True:
    # Kirim data sebagai string ASCII
    uart1.write("7\n".encode('utf-8'))  # Tambahkan newline untuk mudah dibaca
    time.sleep(1)
