import serial.tools.list_ports
import time
import pygame
import math

ESP32_NAME = "ESP32test"
ESP32_MAC = "C8F09E9B7702"

pygame.init()
WIDTH, HEIGHT = 800, 600
screen =  pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("TabbyTank Manual Control")
clock = pygame.time.Clock()
FPS = 60
font = pygame.font.SysFont(None, 24)

def findBluePorts_esp32():
    ports = serial.tools.list_ports.comports()
    esp32_ports = []
    for port in ports:
        if "Bluetooth" in port.description and ESP32_MAC in port.hwid:
            port_type = "Unknown"
            if "Outgoing" in port.description:
                port_type = "Outgoing"
            elif "Incoming" in port.description:
                port_type = "Incoming"
            esp32_ports.append({
                "device": port.device,
                "description": port.description,
                "hwid": port.hwid,
                "type": port_type
            })
    return esp32_ports

def debug_list_ports():
    ports = serial.tools.list_ports.comports()
    print("All available serial ports:")
    for port in ports:
        print(f"  Port: {port.device}, Description: {port.description}, HWID: {port.hwid}")

def BluePort_Link():
    esp32_ports = findBluePorts_esp32()
    if not esp32_ports:
        print("No ESP32 Bluetooth serial ports found.")
    else:
        print("Detected ESP32 Bluetooth serial ports:")
        for port in esp32_ports:
            print(f"  Port: {port['device']}, Description: {port['description']}")
        
    return port['device']
            
    

#basic echo-response communication with ESP32 over BT
""" if __name__ == "__main__":
    port = BluePort_Link()
    ser = serial.Serial(port, 115200, timeout=1)
    while True:
        data = input("> ")
        ser.write((data + "\n").encode())
        response = ser.readline()
        print(f"[ESP32]: {response.decode()}")
    ser.close() """
    
#drawing circle movement indicator UI
CENTRE = (WIDTH // 2, HEIGHT // 2)
RADIUS = 150
colour=(150, 150, 150)
inner_radius = 90
pressed_quadrant = set()

#open serial port
port = BluePort_Link()
while True:
    try:
        ser = serial.Serial(port, baudrate=115200, timeout=1)
        break
    except serial.serialutil.SerialException:
        print(f"Timeout error triggered. Retrying connection to port: {port}")
sendtickrate = 0.25 #send data every 0.5s
starttick = 0
manualmode = False

def draw_quadrant(CENTRE, OUTER_RADIUS, start_angle, end_angle, colour, offset=(0, 0), inner_radius=50):
    # Points along the outer arc
    points = []
    for angle in range(start_angle, end_angle + 1, 1):
        rad = math.radians(angle)
        x = CENTRE[0] + OUTER_RADIUS * math.cos(rad) + offset[0]
        y = CENTRE[1] + OUTER_RADIUS * math.sin(rad) + offset[1]
        points.append((x, y))
    # Points along the inner arc (in reverse)
    for angle in range(end_angle, start_angle - 1, -1):
        rad = math.radians(angle)
        x = CENTRE[0] + inner_radius * math.cos(rad) + offset[0]
        y = CENTRE[1] + inner_radius * math.sin(rad) + offset[1]
        points.append((x, y))
    pygame.draw.polygon(screen, colour, points)
    
    
def get_quadrant(pos, CENTRE, inner_radius, outer_radius):
    dx, dy = pos[0] - CENTRE[0], pos[1] - CENTRE[1]
    dist_sq = dx*dx + dy*dy
    if dist_sq < inner_radius*inner_radius or dist_sq > outer_radius*outer_radius:
        return None  # Outside the button ring
    # Calculate angle and rotate by 45 degrees to match visual quadrants
    angle = (math.degrees(math.atan2(dy, dx)) - 45 + 540) % 360
    if 0 <= angle < 90:
        return 0  # Up
    elif 90 <= angle < 180:
        return 1  # Right
    elif 180 <= angle < 270:
        return 2  # Down
    elif 270 <= angle < 360:
        return 3  # Left
    return None

def get_buttons(pos, length, width):
    return

def darker(colour, factor=0.6):
    return tuple(max(0, int(c * factor)) for c in colour)

    
running = True
while running:
    screen.fill((225, 225, 225))
    # Draw quadrants, using a darker color if pressed
    quad_colours = [colour] * 4
    for q in pressed_quadrant:
        quad_colours[q] = darker(colour)

    draw_quadrant(CENTRE, RADIUS, 45, 135, quad_colours[2], offset=(0, 10), inner_radius=inner_radius)   # Down (2)
    draw_quadrant(CENTRE, RADIUS, 135, 225, quad_colours[3], offset=(-10, 0), inner_radius=inner_radius)  # Left (3)
    draw_quadrant(CENTRE, RADIUS, 225, 315, quad_colours[0], offset=(0, -10), inner_radius=inner_radius) # Up (0)
    draw_quadrant(CENTRE, RADIUS, 315, 405, quad_colours[1], offset=(10, 0), inner_radius=inner_radius) # Right (1)
    pygame.draw.circle(screen, (255, 0, 0), (WIDTH//2, HEIGHT//2), 10)
    
    #manual control mode
    manual_rect = pygame.Rect(650, 60, 100, 80)
    manual_col = (150, 150, 150)
    pygame.draw.rect(screen, manual_col, manual_rect, border_radius=20)
    headMODE = font.render("Manual Mode", True, (0, 0, 0))
    ena = font.render("ENABLED", True, (0, 0, 0))
    dis = font.render("DISABLED", True, (0, 0, 0))
    screen.blit(headMODE, (647, 40))
    if manualmode == False:
        screen.blit(ena, (660, 92))
    else:
        screen.blit(dis, (660, 92))

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            quad = get_quadrant(event.pos, CENTRE, inner_radius, RADIUS)
            if quad is not None:
                pressed_quadrant.add(quad)
                print(f"Clicked quadrant {quad+1}")
            else:
                if manual_rect.collidepoint(event.pos):
                    #update manual col, send command to esp32 saying 'swap'
                    manualmode = not manualmode  # Toggle manual mode
                    print(f"Manual mode is now {'ENABLED' if manualmode else 'DISABLED'}")
                    ser.write(("SWAP\n").encode())  # Send command to ESP32
                    
        elif event.type == pygame.MOUSEBUTTONUP:
            quad = get_quadrant(event.pos, CENTRE, inner_radius, RADIUS)
            if quad is not None:
                pressed_quadrant.discard(quad)
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_w:
                pressed_quadrant.discard(2)  # Remove Down if Up is pressed
                pressed_quadrant.add(0)
            elif event.key == pygame.K_d:
                pressed_quadrant.discard(3)  # Remove Left if Right is pressed
                pressed_quadrant.add(1)
            elif event.key == pygame.K_s:
                pressed_quadrant.discard(0)  # Remove Up if Down is pressed
                pressed_quadrant.add(2)
            elif event.key == pygame.K_a:
                pressed_quadrant.discard(1)  # Remove Right if Left is pressed
                pressed_quadrant.add(3)
            print("Pressed quadrants:", pressed_quadrant)
        elif event.type == pygame.KEYUP:
            if event.key == pygame.K_w:
                pressed_quadrant.discard(0)
            elif event.key == pygame.K_d:
                pressed_quadrant.discard(1)
            elif event.key == pygame.K_s:
                pressed_quadrant.discard(2)
            elif event.key == pygame.K_a:
                pressed_quadrant.discard(3)
            print("Pressed quadrants:", pressed_quadrant)
        #send input data to ESP32
        current_time = time.time()
        if current_time - starttick >= sendtickrate:
            if str(pressed_quadrant) == "set()":
                data = "STOP"
            else:
                halfdata = (str(pressed_quadrant)).strip("{")
                data = (halfdata).strip("}")
            ser.write((data + "\n").encode())
            response = ser.readline()
            print(f"[ESP32]: {response.decode()}")
            starttick = current_time

            
        


    pygame.display.flip()
    clock.tick(FPS)

pygame.quit()
