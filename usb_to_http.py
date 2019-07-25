import serial
import requests 
import time

# usb_device = 'COM'
usb_device = '/dev/tty'
usb_rate = 9600
usb_timeout = 10

# Pushed settings
#  "app_key": "your_app_key"
#  "app_secret": "your_app_secret"
target_type = "app" 
content = ""

def initSerial():
    num = 0
    ser = None
    while num < 30 and not ser:
        try:
            ser = serial.Serial(usb_device+str(num), usb_rate, timeout = usb_timeout)
        except serial.serialutil.SerialException:
            pass
        num+=1
    if not ser:
        return None
    else:
        print(ser.name + " initialized")
        ser.flushInput()
        return ser


def readSerial(ser):
    data = ""
    try:
        data = ser.readline().decode()
        print(data.strip())
    except UnicodeDecodeError:
        print("Decode error")
    return data

def sendNotification(text):
    payload = {
        "app_key": app_key,
        "app_secret": app_secret,
        "target_type": target_type,
        "content": text
    }
    r = requests.post("https://api.pushed.co/1/push", data=payload)
    print("Pushed: " + r.text)

ser = None
while not ser:
    ser = initSerial()

last_message = time.time()
while True:
    if time.time() - last_message >= 60:
        sendNotification("No data for more than a minute")
        last_message = time.time()
    try:
        if ser and ser.inWaiting() > 0:
            data = readSerial(ser)
            if data.startswith("https://"):
                last_message = time.time()
                r = requests.get(data)
                print(r.status_code)
            else:
                sendNotification(data)
    except OSError:
        while not ser:
            ser = initSerial()

