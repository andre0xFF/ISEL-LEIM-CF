import pygame

# inicializes pygame
pygame.init()

# Screen size
screen_size = (400, 500)

screen = pygame.display.set_mode(screen_size)

# frames per second
frame_rate = 10

# clock to control the fps
clock = pygame.time.Clock()

# new frame to show
new_screen = None

#color(tuple with red, green and blue values between 0 and 255)
red = (255, 0, 0)
darker_red = (200, 0, 0)
white = (255, 255, 255)
black =  (0, 0, 0)
green = (0, 255, 0)

pi = 3.14

# main cycle control variable
fim = False

#pygame event function
def processing_pygame_events():
    global fim
    #cycle to process pygame events
    for event in pygame.event.get():
        #event to close the graphic screen
        if event.type == pygame.QUIT:
            fim = True
            
#Builds new frame
def building_new_screen():
    global new_screen
    # creats a new frame
    new_screen = pygame.Surface(screen_size)
    # background collor
    new_screen.fill(black)

def drawing_circle():
    arc1 = pygame.draw.arc(new_screen, red, [75, 75, 250, 250], 0, 2*pi +1, 2) #+1 so it makes a full circle
    

#def speed():
    #Inserir metodo da velocidade !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    #new_screen.blit(velocidade)
    
def button(msg,x,y,w,h,ic,ac):
    mouse = pygame.mouse.get_pos()
    click = pygame.mouse.get_pressed()
    
    
    if x+w > mouse[0] > x and y + h > mouse[1] > y: 
        rect = pygame.draw.rect(new_screen, ac, (x, y, w, h))
        if click[0] == 1:
            print("button pressed")
            speed()
    else:
        rect =pygame.draw.rect(new_screen, ic, (x, y, w, h))
    font = pygame.font.Font("freesansbold.ttf", 20)
    text = font.render(msg, 1, white)
    new_screen.blit(text,rect)



#main cycle
while not(fim):
    processing_pygame_events()
    building_new_screen()
    drawing_circle()
    #speed()
    #button()
    button("Click here",150,350,100,50,darker_red,green)
    # refreshes pygame with new image
    screen.blit(new_screen, (0, 0))
    pygame.display.flip()
    # Only waits the necessary time to accomplish the frame rate
    # must only be called 1 time per frame
    clock.tick(frame_rate)
    
#closes the window
pygame.quit()

