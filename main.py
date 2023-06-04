import pygame
import math

pygame.init()

RADIUS = 10

SUBSTEPS = 10

display = pygame.display.set_mode((700, 700))

# Variable to keep the main loop running

running = True

balls = []
pos = []

clock = pygame.time.Clock()


# Main loop

while running:
    for event in pygame.event.get():
        if event.type == pygame.MOUSEBUTTONDOWN:
            mouse = list(pygame.mouse.get_pos())
            balls.append([[mouse[0], mouse[1]],[0, 0], [0, 0]])
            pos.append([mouse[0], mouse[1]])
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                running = False
        elif event.type == pygame.QUIT:
            running = False


    dt = clock.tick(120)

    for step in range(SUBSTEPS):
        for i in range(len(balls)):

            #gravity
            balls[i][2][1] += 1000

            #contraint
            dist = math.hypot((balls[i][0][0]-350),(balls[i][0][1]-350))
            if dist >= 350-RADIUS:
                n = [(balls[i][0][0]-350)/dist, (balls[i][0][1]-350)/dist]
                dx = n[0]*(dist-300)
                dy = n[1]*(dist-300)
                balls[i][0][0] -= n[0]*(dist-350+RADIUS)
                balls[i][0][1] -= n[1]*(dist-350+RADIUS)
            
            #collision
            for j in range(len(balls)):
                if balls[i][0] != balls[j][0]:
                    collision_axis = [balls[i][0][0]-balls[j][0][0], balls[i][0][1]-balls[j][0][1]]
                    dist = math.hypot(*collision_axis)
                    if dist < RADIUS+RADIUS:
                        n = [collision_axis[0]/dist, collision_axis[1]/dist]
                        delta = 2*RADIUS-dist
                        balls[i][0][0] += n[0]*delta*0.5
                        balls[j][0][0] -= n[0]*delta*0.5
                        balls[i][0][1] += n[1]*delta*0.5
                        balls[j][0][1] -= n[1]*delta*0.5
                        collision_axis = [balls[i][0][0]-balls[j][0][0], balls[i][0][1]-balls[j][0][1]]
                        dist = math.hypot(*collision_axis)

            #integration
            vx = balls[i][0][0] - pos[i][0]
            vy = balls[i][0][1] - pos[i][1]
            pos[i][0] = balls[i][0][0]
            pos[i][1] = balls[i][0][1]
            balls[i][0][0] = balls[i][0][0] + vx + balls[i][2][0]*dt*dt*0.00001/SUBSTEPS/SUBSTEPS
            balls[i][0][1] = balls[i][0][1] + vy + balls[i][2][1]*dt*dt*0.00001/SUBSTEPS/SUBSTEPS
            balls[i][2][0] = 0
            balls[i][2][1] = 0

    display.fill((255, 255, 255))
    pygame.draw.circle(display, (0, 0, 0,), (350, 350), 350, 1)
    for i in balls:
        pygame.draw.circle(display, (0, 0, 0), [i[0][0], i[0][1]], RADIUS, 2)
    pygame.display.flip()