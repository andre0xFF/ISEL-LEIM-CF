...# -*- coding: utf-8 -*-
"""
Created on Fri Jun 23 18:02:05 2017
"""

import pygame
import serial


com = 'COM1'
baudrate = 9600

antialias = True
sonarSound="radarping.wav"
button_file="sonar_or_radar_screen_mini_button.png"
icon_file="icon.png"
background_file="sonar1.jpg"

pygame.init()

#-----------------class Background---------------------
class Background:
    #background initialization
    def __init__(self, icon_image, background_image, size_X, size_Y):
        self.background_image = background_image
        self.size_x = size_X
        self.size_y = size_Y
        self.icon_image = icon_image
        self.surface = pygame.display.set_mode((self.size_x, self.size_y))

    #return dimensions of background
    def get_Dimensions(self):
        size = []
        size.append(self.size_x)
        size.append(self.size_y)
        return size
    #return background size along axis X
    def get_sizeX(self):
        return self.size_x
    
    #sets background size along axis X
    def set_sizeX(self,size_x):
        self.size_x=size_x
    
    #return background size along axis y
    def get_sizeY(self):
        return self.size_y
    
    #sets background size along axis y
    def set_sizeY(self,size_y):
        self.size_y=size_y
    
    #sets background size along axis X
    def set_Background_Image(self,image):
        self.background_image = image
        
    #return background image
    def get_Background_Image(self):
        return self.background_image   
    
    #sets background Icon image
    def set_Icon_Image(self,icon):
        self.icon_image = icon
    
    #return background Icon image
    def get_Icon_Image(self):
        return self.icon_image
    
    #sets background 
    def set_Background(self):
        
        
        self.surface.fill((255, 255, 255))
        
        if self.icon_image is not None:
            
            icon = pygame.image.load(self.icon_image)
            pygame.display.set_icon(icon)
        
        if self.background_image is not None:
            
            background_image = pygame.image.load(self.background_image).convert_alpha()
            self.surface.blit(background_image, (0, 0))
        pygame.display.update()


    #returns background surface
    def get_Surface(self):
        return self.surface

#-----------------class Button---------------------
class Button:
    #button initialization
    def __init__(self,button_image, center_X, center_y):
        self.button_image = button_image
        self.center_X = center_X
        self.center_y = center_y
        image = pygame.image.load(self.button_image)
        imagerect = image.get_rect()
        imagerect.center = self.get_coords()
        self.button_box=imagerect
  

    #returns center coordinates of the button
    def get_coords(self):
        return(self.get_center_positionX(), self.get_center_positionY())
    
    #returns center of coordinate X
    def get_center_positionX(self):
        return self.center_X
    
    #sets center coordinate X
    def set_center_cordX(self,center_X):
        self.center_X=center_X
    
    #returns center of coordinate y
    def get_center_positionY(self):
        return self.center_y
    
    #sets center coordinate y
    def set_center_cordY(self,center_y):
        self.center_y=center_y
     
    #sets button image
    def set_button_image(self,image):
        self.button_image = image
        
    #returns button image
    def get_button_image(self):
        return self.button_image   
    
    #sets button on choosen surface
    def set_button(self,surface):
        surf=surface
        image = pygame.image.load(self.button_image)
        imagerect = image.get_rect()
        imagerect.center = self.get_coords()
        surf.blit(image, imagerect)
        
    #returns button box coordinates
    def get_button_box(self):
        return self.button_box
    
    #returns true if button pressed or false otherwise
    def button_pressed(self):
        if pygame.mouse.get_pressed()[0] == True:
            m_x,m_y=pygame.mouse.get_pos()
            x_left=self.button_box[0]
            y_top=self.button_box[1]
        
            width=self.button_box[2]
            x_rigth=x_left+width
            height=self.button_box[3]
            y_bot=y_top+height
            
            #verification if button was clicked
            if m_x > x_left and m_x < x_rigth and m_y > y_top and m_y < y_bot:
    
                print("GUI Button was clicked")
                return True
    
        return False
        
#plays designated sound
def sound(sound):
    pygame.mixer.init()
    pygame.mixer.music.load(sound)
    pygame.mixer.music.play()
        


# prints text to the selected coords of the background screen
def textLabel(screen, text,coords):
    
    myfont = pygame.font.SysFont("monospace", 50)

    
    label = myfont.render(text, 1, (255,255,0))
    screen.blit(label, coords)
    
#initializes and renders the user interface elements
def GUI():
    screen=Background(icon_file,background_file,1000,750)
    screen.set_Background()
    
    
    button=Button(button_file,500, 600)
    button.set_button(screen.get_Surface())

    pygame.display.update()
    guiArray=[]
    guiArray.append(screen)
    guiArray.append(button)
    
    #returns array containing the elements
    return guiArray

    
def main():
    #initialize port for serial communication
    Serie = comInit(com, baudrate)
    
    
    gui=GUI()
    button=gui[1]
    textLabel(gui[0].get_Surface(),"Velocity: Standby",(gui[0].get_sizeX()/4.,gui[0].get_sizeY()/3.))
    pygame.display.update()


    running = True

    #loop that keeps checking any changes or commands to obtain velocity info
    while running:
        #checks if the interface button is pressed and prints velocity data on Interface
        if button.button_pressed():
            
                #array that contains current screen and button
                gui=GUI()
                button=gui[1]
                
                #Sends to the port in this case to arduino the character R
                caracterSend(Serie,'R')
                
                #prints on the designated screen the information(velocity) received from arduino
                textLabel(gui[0].get_Surface(),"Velocity:"+stringReceive(Serie),(gui[0].get_sizeX()/4.,gui[0].get_sizeY()/3.))
                
                #plays sonar emmitter sound
                sound(sonarSound)
                pygame.display.update()
                
                #checks if it exists characters waiting to be read from the com and prints velocity data on Interface
        if Serie.inWaiting()>0:
            
                #array that contains current screen and button
                gui=GUI()
                button=gui[1]
                
                #prints on the designated screen the information(velocity) received from arduino
                textLabel(gui[0].get_Surface(),"Velocity:"+stringReceive(Serie),(gui[0].get_sizeX()/4.,gui[0].get_sizeY()/3.))
                
                #plays sonar emmitter sound
                sound(sonarSound)
                pygame.display.update()
                
        for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    print("quiting")
                    running = False
    pygame.quit()


# initialization of the port connected to arduino
def comInit(com, baudrate):

    try:
        Serie = serial.Serial(com, baudrate)
        print ('Successful connection to Arduino.')
        print ('Connected to ' + Serie.portstr)
        return Serie
    except Exception as e:
        print ('Unsuccessful connection to Arduino.')
        print (e)
    return None

#Recieves a character from the port(Arduino)
def caracterReceive(Serie):
    try:
        return Serie.read()
    except:
        print ('Communication Error.')
    Serie.close()

#Sends a character trough the port(to arduino)
def caracterSend(Serie, info):

    try:
        #encodes the desired string into bytes type
        string_encoded=info.encode("UTF-8")
        

        #Writes or sends the string trough the port
        Serie.write(string_encoded)

    except:
        print ('Communication Error2.')
        Serie.close()

def stringReceive(Serie):
    try:
        
        #decodes the byte type into string with the designated encoding
        string_decoded=Serie.readline().decode("UTF-8")
        

        #removes spaces at the end of message
        sentence=string_decoded[:len(string_decoded)-2]

        return sentence
    except:
        print ('Communication Error1.')
        Serie.close()

if __name__ == '__main__':
    main()
