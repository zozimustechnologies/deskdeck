import serial
import psutil
import time
import subprocess

# ================= SERIAL PORT =================
PORT = "/dev/cu.usbmodem1101"

arduino = serial.Serial(PORT, 9600)

print("DeskDeck Connected")

last_cmd = ""

# ================= CHECK APP RUNNING =================
def is_running(app_name):

    script = f'''
    tell application "System Events"
        return (name of processes) contains "{app_name}"
    end tell
    '''

    result = subprocess.check_output([
        "osascript",
        "-e",
        script
    ]).decode().strip()

    return result == "true"

# ================= MAIN LOOP =================
while True:

    # ================= SEND BROWSER STATUS =================
    if is_running("Safari"):
        arduino.write(b"SAFARI_OPEN\n")
    else:
        arduino.write(b"SAFARI_CLOSED\n")

    if is_running("Microsoft Edge"):
        arduino.write(b"EDGE_OPEN\n")
    else:
        arduino.write(b"EDGE_CLOSED\n")

    if arduino.in_waiting:

        cmd = arduino.readline().decode().strip()

        if cmd != last_cmd:
            print("Received:", cmd)

        last_cmd = cmd

        # ================= PLAY / PAUSE =================
        if cmd == "MEDIA_PLAY":

            script = '''
            tell application "Spotify"
                playpause
            end tell
            '''

            subprocess.run([
                "osascript",
                "-e",
                script
            ])

        # ================= NEXT TRACK =================
        elif cmd == "MEDIA_NEXT":

            script = '''
            tell application "Spotify"
                next track
            end tell
            '''

            subprocess.run([
                "osascript",
                "-e",
                script
            ])

        # ================= PREVIOUS TRACK =================
        elif cmd == "MEDIA_PREV":

            script = '''
            tell application "Spotify"
                previous track
            end tell
            '''

            subprocess.run([
                "osascript",
                "-e",
                script
            ])

        # ================= OPEN SAFARI =================
        elif cmd == "OPEN_SAFARI":

            subprocess.run([
                "open",
                "-a",
                "Safari"
            ])

        # ================= CLOSE SAFARI =================
        elif cmd == "CLOSE_SAFARI":

            subprocess.run([
                "osascript",
                "-e",
                'tell application "Safari" to quit'
            ])

        # ================= OPEN EDGE =================
        elif cmd == "OPEN_EDGE":

            subprocess.run([
                "open",
                "-a",
                "Microsoft Edge"
            ])

        # ================= CLOSE EDGE =================
        elif cmd == "CLOSE_EDGE":

            subprocess.run([
                "osascript",
                "-e",
                'tell application "Microsoft Edge" to quit'
            ])

        # ================= OPEN STEAM =================
        elif cmd == "OPEN_STEAM":

            subprocess.run([
                "open",
                "-a",
                "Steam"
            ])

        # ================= CPU =================
        elif cmd == "GET_CPU":

            cpu = int(psutil.cpu_percent())

            line1 = "CPU Load"
            line2 = f"{cpu}%"

            message = f"{line1}|{line2}\n"

            arduino.write(message.encode())

        # ================= RAM =================
        elif cmd == "GET_RAM":

            ram = int(psutil.virtual_memory().percent)

            line1 = "RAM Usage"
            line2 = f"{ram}%"

            message = f"{line1}|{line2}\n"

            arduino.write(message.encode())

        # ================= STORAGE =================
        elif cmd == "GET_STORAGE":

            disk = psutil.disk_usage('/')

            used_gb = int(disk.used / (1024**3))
            total_gb = int(disk.total / (1024**3))

            line1 = "Storage"
            line2 = f"{used_gb}/{total_gb}GB"

            message = f"{line1}|{line2}\n"

            arduino.write(message.encode())

    time.sleep(0.3)
