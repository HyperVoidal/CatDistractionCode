import serial.tools.list_ports
import serial
import time

HC05HWID = '001403050925'

def debug_list_ports():
    ports = serial.tools.list_ports.comports()
    print("All available serial ports:")
    for port in ports:
        print(f"  Port: {port.device}, Description: {port.description}, HWID: {port.hwid}")
        
def findBlueMod():
    ports = serial.tools.list_ports.comports()
    bt_ports = []
    
    for port in ports:
        if "Standard Serial over Bluetooth link" in port.description:
            mac_match = re.search(r'&([0-9A-F]{12})_C00000000', port.hwid, re.IGNORECASE)
            if mac_match:
                mac = mac_match.group(1)
                # Filter out invalid MACs (like all zeroes)
                if mac != "000000000000":
                    bt_ports.append([port.device, mac])
    
    print(bt_ports)
    a = 0
    for i in bt_ports:
        if bt_ports[a][1] == HC05HWID:
            return str(bt_ports[a][0])
        a += 1

debug_list_ports()
CPort = findBlueMod()
print(CPort)

#open serial port
port = findBlueMod()
retries = 10
while retries > 0:
    try:
        ser = serial.Serial(port, baudrate=9600, timeout=1)
        break
    except serial.serialutil.SerialException:
        print(f"Retrying connection to port: {port}")
        time.sleep(2)
        retries -= 1
if retries == 0:
    print("Failed to connect to Bluetooth device. Exiting.")
    exit()