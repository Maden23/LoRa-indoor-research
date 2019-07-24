import serial
import requests 

# usb_device = 'COM'
usb_device = 'tty'
usb_rate = 9600
usb_timeout = 10

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
        return None, num
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

ser = None
while not ser:
    ser = initSerial()

while True:
    try:
        if ser and ser.inWaiting() > 0:
            data = readSerial(ser)
            if data.startswith("https://"):
                r = requests.get(data)
                print(r.status_code)
    except OSError:
        while not ser:
            ser = initSerial()

