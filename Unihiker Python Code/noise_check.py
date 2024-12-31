from pinpong.extension.unihiker import *
from pinpong.board import Board, Pin
from unihiker import Audio
from unihiker import GUI
import time

Board().begin()

# Instantiate objects
audio = Audio()
gui = GUI()

# Background color
gui.fill_rect(x=0, y=0, w=240, h=320, color="#ebffde")

# Title section
gui.draw_text(text="Goat Guards", x=120, y=30, font_size=24, color="#FF4500", origin="center")
gui.draw_text(text="System", x=120, y=60, font_size=24, color="#FF4500", origin="center")

# Add decorative separator
gui.draw_line(x0=20, y0=90, x1=220, y1=90, width=2, color="#FF4500")

# Labels for sound value
gui.draw_text(text="Sound Level:", x=120, y=120, font_size=20, color="#000000", origin="center")
sound_text = gui.draw_text(text="", x=120, y=170, font_size=55, color="#FF4500", origin="center")

# Footer text
gui.draw_text(text="Monitoring Active", x=120, y=300, font_size=14, color="#008000", origin="center")

# Counter
start_time = time.time()
i = 0
sound_levels = [0] * 10
threshold = 18
count_text = gui.draw_text(text="", x=120, y=220, font_size=12, color="#FF4500", origin="center")

# Print Column Name
detection_string = f"timestamp, total_detections, data_0, data_1, data_2, data_3, data_4, data_5, data_6, data_7, data_8, data_9\n"
with open("goat_guards_data.txt", "a") as file:
    file.write(detection_string)

while True:
    # Calculate the current second
    elapsed_time = time.time() - start_time
    second_index = int(elapsed_time % 10)

    # Update the displayed sound value
    sound_value = audio.sound_level()
    sound_text.config(text=sound_value)

    if (sound_value > threshold):
        sound_levels[i] = 1

    int_time = int(elapsed_time)

    if i != int_time:
        status = "Detected" if sound_levels[i] == 1 else "Not Detected"
        print(f"Time now = {i}")
        print(f"Status = {status}")
        i = i + 1
    
    if (i>9):
        total_detections = sum(sound_levels)  # Sum all values in sound_levels
        print(f"Total Detections: {total_detections}\n\n")

        timestamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        detection_string = f"{timestamp}, {total_detections}, {sound_levels[0]}, {sound_levels[1]}, {sound_levels[2]}, {sound_levels[3]}, {sound_levels[4]}, {sound_levels[5]}, {sound_levels[6]}, {sound_levels[7]}, {sound_levels[8]}, {sound_levels[9]}\n"
        with open("goat_guards_data.txt", "a") as file:
            file.write(detection_string)

        i = 0
        start_time = time.time()
        sound_levels = [0] * 10
        count_text.config(text=f"Count: {total_detections}")

    # Add a small sleep to prevent high CPU usage
    time.sleep(0.1)
