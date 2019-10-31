'''
    Python script for monitoring and controlling reflow oven

    This script will receive temperature information through serial
    port from the reflow oven.
    Then we will send following temperature to reach as well as holding time

'''

import glob
import serial
import time

DEFAULT_PORT = None
DEFAULT_BAUDRATE = 9600
DEFAULT_RTS = False
DEFAULT_DTR = False
DEFAULT_BYTESIZE = serial.EIGHTBITS
DEFAULT_PARITY = serial.PARITY_NONE
DEFAULT_STOPBITS = serial.STOPBITS_ONE
DEFAULT_RDTIMEOUT = None
DEFAULT_FLOWCONTROL = False
DEFAULT_WRTIMEOUT = None
DEFAULT_INTERCHARTIMEOUT = None

openPort = False
comPortStr = "COM"

# Obtain list of COM ports
# Should for any NIX system

ports = glob.glob('/dev/tty.*')

print("Ports on this Mac")

for num, serialP in enumerate(ports):
    print(num, serialP)

ser = serial.Serial(
    port=DEFAULT_PORT,
    baudrate=DEFAULT_BAUDRATE,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

while openPort == False:
    #Get COM port
    comPortNum = raw_input("Choose COM port from list and type it in\n")

    DEFAULT_PORT = ports[int(comPortNum)]

    ser.port = DEFAULT_PORT
    try:
        ser.open()

    except Exception, e:
        print("error opening serial port: " + str(e) + "\n")
        openPort = False

    if ser.isOpen():
        openPort = True
        print("Serial Open")
        ser.flushInput()  # flush input buffer, discarding all its contents
        ser.flushOutput()  # flush output buffer, aborting any current output

    else:
        print("Cannot open serial port \n" + DEFAULT_PORT)

loop = True

while loop:
    print("Sending commands")
    userInput = raw_input("Type any string of characters \r\n \
                            Or type 'quit' to end\r\n")
    if userInput == "quit":
        loop = False
    ser.write(userInput + '\r')
    time.sleep(1)
    outputStr = ''
    while ser.inWaiting() > 0:
        outputStr += ser.read(1)
    if outputStr != '':
        print(outputStr)

ser.close()
