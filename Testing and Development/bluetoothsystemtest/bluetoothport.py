import serial.tools.list_ports
import serial
import time
import pygame
import math
import re

HC05HWID = '001403050925'

pygame.init()
WIDTH, HEIGHT = 800, 600
screen =  pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("TabbyTank Manual Control")
clock = pygame.time.Clock()
FPS = 60
font = pygame.font.SysFont(None, 24)

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
    
#drawing circle movement indicator UI
CENTRE = (WIDTH // 2, HEIGHT // 2)
RADIUS = 150
colour=(150, 150, 150)
inner_radius = 90
pressed_quadrant = set()

last_valid_data = None
last_input_time = 0
hold_delay = 0.5  # half a second to retain last input

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
sendtickrate = 0.5 #send data every 0.25s
starttick = 0
manualmode = False
lasermode = False

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
    enaMAN = font.render("ENABLE", True, (0, 0, 0))
    disMAN = font.render("DISABLE", True, (0, 0, 0))
    screen.blit(headMODE, (647, 40))
    if manualmode == False:
        screen.blit(enaMAN, (660, 92))
    else:
        screen.blit(disMAN, (660, 92))
    
    #laser pointer control button
    laser_rect = pygame.Rect(133, 60, 100, 80)
    laser_col = (150, 150, 150)
    pygame.draw.rect(screen, laser_col, laser_rect, border_radius=20)
    laserMODE = font.render("Laser Mode", True, (0, 0, 0))
    enaLAS = font.render("ENABLE", True, (0, 0, 0))
    disLAS = font.render("DISABLE", True, (0, 0, 0))
    screen.blit(laserMODE, (137, 40))
    if lasermode == False:
        screen.blit(enaLAS, (140, 92))
    else:
        screen.blit(disLAS, (140, 92))
        
    #Emergency pause button
    emer_rect = pygame.Rect(350, 60, 100, 80)
    emer_col = (150, 150, 150)
    pygame.draw.rect(screen, emer_col, emer_rect, border_radius=20)
    tag = font.render("EMERGENCY STOP (5s)", True, (0, 0, 0))
    screen.blit(tag, (357, 92))

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
                    #update manual col, send command to Arduino saying 'swap'
                    manualmode = not manualmode  # Toggle manual mode
                    print(f"Manual mode is now {'ENABLED' if manualmode else 'DISABLED'}")
                    ser.write(("SWAP\n").encode())  # Send command to Arduino
                if laser_rect.collidepoint(event.pos):
                    #update laserpoint col, send command to arduino saying 'LASER'
                    lasermode = not lasermode
                    print(f"Laser is now {'ENABLED' if lasermode else 'DISABLED'}")
                    ser.write(("LASER\n").encode())
                if emer_rect.collidepoint(event.pos):
                    #Send in emergency brake signal
                    print(f"EMERGENCY STOP ACTIVATED")
                    ser.write(("STOP\n").encode())
                    
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


    #send input data to Arduino - isolated from event check loop
    current_time = time.time()
    if current_time - starttick >= sendtickrate:
        if str(pressed_quadrant) == "set()":
            if time.time() - last_input_time < hold_delay and last_valid_data:
                data = last_valid_data  # re-send last data
            else:
                data = "N/A"
        else:
            halfdata = (str(pressed_quadrant)).strip("{")
            data = (halfdata).strip("}")
            last_valid_data = data
            last_input_time = time.time()
        ser.write((data + "\n").encode())
        response = ser.readline()
        try:
            print(f"[Arduino]: {response.decode('utf-8')}")
        except UnicodeDecodeError:
            print("[Arduino]: Corruption error - cannot decode message. Bypassing for integrity.")
        starttick = current_time
            
        


    pygame.display.flip()
    clock.tick(FPS)

pygame.quit()
