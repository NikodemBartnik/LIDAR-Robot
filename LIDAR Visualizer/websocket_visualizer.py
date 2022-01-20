from tabnanny import check
from tkinter import E
from turtle import distance
import pygame, math, serial, numpy
import asyncio
import websockets
import keyboard
pygame.init()

#constant based on lidar resolution
LIDAR_RESOLUTION = 80
VISUALIZATION_RESOLUTION = 80

#global variables
distances_list = []
mode = 'm'
bs = 0
first_run = True

async def recv(websocket):
    global distances_list, first_run
    async for message in websocket:
        d = message.split(",")[:-1]
        if first_run:
            distances_list = d
            first_run = False
        else:
            distances_list = filterDistancesList(d)
        await GameUpdate()
        await websocket.send(checkKeys())


def GenerateLinePositions(numberOfLines):
    angle = 360/numberOfLines
    lines = []
    for x in range(numberOfLines):
        lines.append([300 * math.cos((x)*angle/180 * math.pi), 300 * math.sin((x)*angle/180 * math.pi)])
        #lines.append([300 * math.sin((x)*angle/180 * math.pi), 300 * math.cos((x)*angle/180 * math.pi)])
    return lines

def checkKeys():
    global mode
    if keyboard.is_pressed('a'):
        mode = 'a'
    elif keyboard.is_pressed('m'):
        mode = 'm'

    if mode is 'a':
        return directionAlgorithm()
    elif mode is 'm':
        if keyboard.is_pressed('up'):
            return 'forward'
        elif keyboard.is_pressed('down'):
            return 'stop'
        elif keyboard.is_pressed('left'):
            return 'left'
        elif keyboard.is_pressed('right'):
            return 'right'
        else:
            return 'stop'

def directionAlgorithm():
    global bs
    alg_dir = 'stop'
    biggestSpace = [0,0]
    for a in range(int(len(distances_list)/2)-10):
        sum = 1
        for b in range(10):
            sum += int(distances_list[2*a+2*b]) * int(distances_list[2*a+2*b+1])
        if biggestSpace[1] < sum:
            biggestSpace[0] = a+5
            biggestSpace[1] = sum
            bs = a*2
    if (biggestSpace[0] > 60) or biggestSpace[0] < 20:
        alg_dir = 'left'
    elif biggestSpace[0] > 20 and biggestSpace[0] < 50:
        alg_dir = 'right'
    else:
        alg_dir = 'forward'
    return alg_dir

def filterDistancesList(d_list):
    filtered_list =[]
    for i in range(int(len(d_list)/2)):
        if int(d_list[i*2+1]) < 100:
            filtered_list.append(distances_list[i*2])
            filtered_list.append(distances_list[i*2+1])
        elif int(d_list[i*2]) > 7500:
            filtered_list.append(distances_list[i*2])
            filtered_list.append(distances_list[i*2+1])
        else:
            filtered_list.append(d_list[i*2])
            filtered_list.append(d_list[i*2+1])
    return filtered_list

def map(val, in_min, in_max, out_min, out_max):
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min



line_positions = GenerateLinePositions(VISUALIZATION_RESOLUTION)
for l in line_positions:
        print('(', l[0], ',', l[1], ')')

# Set up the drawing window
screen = pygame.display.set_mode([800, 800])
sysfont = pygame.font.get_default_font()
font1 = pygame.font.SysFont(sysfont, 72)


async def GameUpdate():
    
    if(len(distances_list) == VISUALIZATION_RESOLUTION*2):
    
        # Did the user click the window close button?
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            
        # Fill the background with white
        screen.fill((20, 10, 30))
        print('bs: ', bs)

        #print('Front distance: ', distances_list[120])
        for x in range(VISUALIZATION_RESOLUTION):
            a = int(distances_list[x*2])/2000
            #if a > 2:
               # print('Error: ', distances_list[x*2])
            #print('Color: ', distances_list[x*2+1])
            #pygame.draw.aaline(screen, pygame.Color(160,255,255), (400, 400), (numpy.array(line_positions[x])*a) + (400, 400), 4)
            #pygame.draw.aaline(screen, (int(distances_list[x*2+1]),int(distances_list[x*2+1]),int(distances_list[x*2+1])), (400, 400), (numpy.array(line_positions[x])*a) + (400, 400), 4)
            if (mode is 'a') and map(bs, 0, 160, 80, 0) == x:
                pygame.draw.aaline(screen, (0,255,0), (400, 400), (line_positions[x][0]+400, line_positions[x][1]+400), 4)
            else:
                pygame.draw.aaline(screen, (int(distances_list[x*2+1]),int(distances_list[x*2+1]),int(distances_list[x*2+1])), (400, 400), (line_positions[x][0]*a+400, line_positions[x][1]*a+400), 4)

        pygame.draw.circle(screen, pygame.Color(100, 100, 120), (400, 400), 60)
        # Flip the display
        pygame.display.flip()


async def main():
    async with websockets.serve(recv, "192.168.0.107", 8084):
        await asyncio.Future()  # run forever

asyncio.run(main())