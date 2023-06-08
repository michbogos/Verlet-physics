import pygame
import math
import random
import time
import threading

from multiprocessing import Process


pygame.init()

RADIUS = 10
SUBSTEPS = 8

GRID_SUBDIVISIONS = 20

RADIUS_CONTAINER = 300

display = pygame.display.set_mode((700, 700))

balls = []

grid = [[[] for i in range(math.ceil(700/GRID_SUBDIVISIONS))] for j in range(math.ceil(700/GRID_SUBDIVISIONS))]
grid_size = math.ceil(700/GRID_SUBDIVISIONS)


class Ball:
    def __init__(self, x, y, color):
        self.pos_current = [x, y]
        self.pos_prev = [x, y]
        self.vel = [0, 0]
        self.acc = [0, 0]
        self.checked = False
        self.pos_grid = [x//GRID_SUBDIVISIONS, y//GRID_SUBDIVISIONS]
        self.color = color

    def accelerate(self):
        self.acc[1] += 1000
    
    def solve_constraint(self):
        dist = math.hypot((self.pos_current[0]-350),(self.pos_current[1]-350))
        if dist >= RADIUS_CONTAINER-RADIUS:
            n = [(self.pos_current[0]-350)/dist, (self.pos_current[1]-350)/dist]
            dx = n[0]*(dist-(350-RADIUS))
            dy = n[1]*(dist-(350-RADIUS))
            self.pos_current[0] -= n[0]*(dist-RADIUS_CONTAINER+RADIUS)
            self.pos_current[1] -= n[1]*(dist-RADIUS_CONTAINER+RADIUS)
    
    def solve_collision(self, ball):
        if ball.pos_current != self.pos_current:
            collision_axis = [self.pos_current[0]-ball.pos_current[0], self.pos_current[1]-ball.pos_current[1]]
            dist = math.hypot(*collision_axis)
            if dist < RADIUS+RADIUS:
                n = [collision_axis[0]/dist, collision_axis[1]/dist]
                delta = 2*RADIUS-dist
                self.pos_current[0] += n[0]*delta*0.5
                ball.pos_current[0] -= n[0]*delta*0.5
                self.pos_current[1] += n[1]*delta*0.5
                ball.pos_current[1] -= n[1]*delta*0.5
                collision_axis = [self.pos_current[0]-ball.pos_current[0], self.pos_current[1]-ball.pos_current[1]]
    
    def integrate(self, dt):
        vx = self.pos_current[0] - self.pos_prev[0]
        vy = self.pos_current[1] - self.pos_prev[1]
        self.pos_prev[0] = self.pos_current[0]
        self.pos_prev[1] = self.pos_current[1]
        self.pos_current[0] = self.pos_current[0] + vx + self.acc[0]*dt*dt
        self.pos_current[1] = self.pos_current[1] + vy + self.acc[1]*dt*dt
        self.acc[0] = 0
        self.acc[1] = 0

        

# Variable to keep the main loop running

running = True

clock = pygame.time.Clock()

object_count = 0

def collision():
    for row in range(1, grid_size-1):
            for cell in range(1, grid_size-1):
                for dx in range(-1, 2):
                    for dy in range(-1, 2):
                        #print(container)
                        for ball1 in grid[row][cell]:
                            for ball2 in grid[row+dx][cell+dy]:
                                #checks += 1
                                ball1.solve_collision(ball2)


def collision1():
    for row in range(1, (grid_size-1)//2):
            for cell in range(1, grid_size-1):
                for dx in range(-1, 2):
                    for dy in range(-1, 2):
                        #print(container)
                        for ball1 in grid[row][cell]:
                            for ball2 in grid[row+dx][cell+dy]:
                                #checks += 1
                                ball1.solve_collision(ball2)

def collision2():
    for row in range((grid_size//2), grid_size-1):
            for cell in range(1, grid_size-1):
                for dx in range(-1, 2):
                    for dy in range(-1, 2):
                        #print(container)
                        for ball1 in grid[row][cell]:
                            for ball2 in grid[row+dx][cell+dy]:
                                #checks += 1
                                ball1.solve_collision(ball2)


# Main loop
frame = 0
while running:
    frame +=1
    if frame % 10 == 0:
        grid[340//GRID_SUBDIVISIONS][70//GRID_SUBDIVISIONS].append(Ball(340, 70, [random.randrange(0, 255), random.randrange(0, 255), random.randrange(0, 255)]))
        object_count += 1
    for event in pygame.event.get():
        if event.type == pygame.MOUSEBUTTONDOWN:
            # mouse = list(pygame.mouse.get_pos())
            # grid[mouse[0]//GRID_SUBDIVISIONS][mouse[1]//GRID_SUBDIVISIONS].append(Ball(mouse[0], mouse[1]))
            #balls.append(Ball(mouse[0], mouse[1]))
            pass
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                running = False
        elif event.type == pygame.QUIT:
            running = False


    dt = clock.tick(30)
    checks = 0
    for step in range(SUBSTEPS):
        col1 = time.perf_counter()
        collision()
        col2 = time.perf_counter()

        rest1 = time.perf_counter()
        for row in range(len(grid)):
            for cell in range(len(grid[row])):
                for ball in range(len(grid[row][cell])):
                    grid[row][cell][ball].accelerate()
                    grid[row][cell][ball].solve_constraint()
                    grid[row][cell][ball].integrate(dt/1000/SUBSTEPS)
        rest2 = time.perf_counter()

        grid1 = time.perf_counter()
        row = 0
        while row < len(grid):
            cell = 0
            while cell < len(grid[row]):
                ball = 0
                it = 0
                while ball < len(grid[row][cell]):
                    if grid[row][cell][ball].pos_current[0]//GRID_SUBDIVISIONS != grid[row][cell][ball].pos_grid[0] or grid[row][cell][ball].pos_current[1]//GRID_SUBDIVISIONS != grid[row][cell][ball].pos_grid[1]:
                            obj = grid[row][cell][ball]
                            del grid[row][cell][ball]
                            ball -= 1
                            obj.pos_grid = [int(obj.pos_current[0]//GRID_SUBDIVISIONS),int(obj.pos_current[1]//GRID_SUBDIVISIONS)]
                            grid[obj.pos_grid[0]][obj.pos_grid[1]].append(obj)
                    ball += 1
                cell += 1
            row += 1
        grid2 = time.perf_counter()


    display.fill((255, 255, 255))
    pygame.draw.circle(display, (0, 0, 0), (350, 350), RADIUS_CONTAINER, 1)
    # for i in range(math.ceil(700/GRID_SUBDIVISIONS)):
    #         pygame.draw.line(display, (255, 0, 0), (i*GRID_SUBDIVISIONS, 0), (i*GRID_SUBDIVISIONS, 700))
    # for i in range(math.ceil(700/GRID_SUBDIVISIONS)):
    #         pygame.draw.line(display, (255, 0, 0), (0, i*GRID_SUBDIVISIONS), (700, i*GRID_SUBDIVISIONS))
    # for row in range(len(grid)):
    #     for cell in range(len(grid[row])):
    #         if len(grid[row][cell]) > 0:
    #             pygame.draw.rect(display, (0, 255, 255), (row*GRID_SUBDIVISIONS, cell*GRID_SUBDIVISIONS, GRID_SUBDIVISIONS, GRID_SUBDIVISIONS))
    draw1 = time.perf_counter()
    for row in grid:
        for cell in row:
            for ball in cell:
                pygame.draw.circle(display, ball.color, ball.pos_current, RADIUS)
    pygame.display.flip()
    draw2 = time.perf_counter()
    if frame%100 == 0:
        print(f"{object_count}: {checks}")
        print(f"Collision_time: {col2-col1}")
        print(f"Other_time: {rest2-rest1}")
        print(f"Grid: {grid2-grid1}")
        print(f"Draw: {draw2-draw1}")