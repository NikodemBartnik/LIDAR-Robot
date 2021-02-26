import pygame, math, serial, numpy
pygame.init()

#constant based on lidar resolution
LIDAR_RESOLUTION = 188
#lidar resolution divided by 4 to simplify the visualization
VISUALIZATION_RESOLUTION = 188


def GetDataFromArduino():
    #[:-3] get rid of end of line sign and additional comma separator that is sent from arduino
    data = arduino.readline()[:-3].decode("utf-8") 
    distances_list = data.split(",")
    return distances_list


def GenerateLinePositions(numberOfLines):
    angle = 360/numberOfLines
    lines = []
    for x in range(numberOfLines):
        lines.append([300 * math.cos((x+1)*angle/180 * math.pi), 300 * math.sin((x+1)*angle/180 * math.pi)])
    return lines


def SimplifyMeasurments(distances):
    simplifiedDistances = []
    d = LIDAR_RESOLUTION/VISUALIZATION_RESOLUTION
    for x in range(VISUALIZATION_RESOLUTION):
        distance = 0
        for a in range(int(d)):
            
            distance += int(distances[x*4-int(d)])
        simplifiedDistances.append(distance/d)
    return simplifiedDistances



arduino = serial.Serial('COM5', 230400, timeout=.1)

line_positions = GenerateLinePositions(VISUALIZATION_RESOLUTION)
print(line_positions)

# Set up the drawing window
screen = pygame.display.set_mode([800, 800])
sysfont = pygame.font.get_default_font()
font1 = pygame.font.SysFont(sysfont, 72)

while True:
    distances = GetDataFromArduino()
    if(len(distances) == 188):
    
        # Did the user click the window close button?
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            
    
        # Fill the background with white
        screen.fill((20, 10, 30))

    
        for x in range(VISUALIZATION_RESOLUTION):
            a = int(distances[x])/2000
            pygame.draw.aaline(screen, (160,160,180), (400, 400), (numpy.array(line_positions[x])*a) + (400, 400), 4)
            
            
        pygame.draw.circle(screen, (100, 100, 120), (400, 400), 60)
        # Flip the display
        pygame.display.flip()

arduino.close()
pygame.quit()

