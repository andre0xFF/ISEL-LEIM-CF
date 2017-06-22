# -*- coding: utf-8 -*-
"""
Created on Wed Jun 21 16:39:53 2017

@author: Danie
"""

import pygame
import serial
#import os

com = 'COM1'
baudrate = 9600

antialias = True

#class Background(pygame.sprite.Sprite):
#    def __init__(self, image_file, location):
#        pygame.sprite.Sprite.__init__(self)  #call Sprite initializer
#        self.image = pygame.image.load(image_file)
#        self.rect = self.image.get_rect()
#        self.rect.left, self.rect.top = location


def Button(Picture, coords, surface):
    print("buttonify")
    image = pygame.image.load(Picture)
    imagerect = image.get_rect()
#    print(imagerect)
    imagerect.center = coords
#    print(imagerect.center)
    surface.blit(image,imagerect)
    return (image,imagerect)

def freeText(screen,text, x, y, color, size, font):
    print("Entrou")
    fontType = pygame.font.SysFont(font, size)
    textDisplay = fontType.render(text, antialias, color)
    textpos = textDisplay.get_rect()
    textpos.center = (x,y)
    screen.blit(textDisplay, textpos)

    # Componentes adicionais, não necessárias para o funcionamento da função apenas para ajuda na composição do jogo de modo a conhecer a posição ocupada pelo texto
    rect_coordinates = [textpos.topleft, textpos.bottomright]
    return rect_coordinates
        
def GUI():
#    try:
     array_GUI=[]
     pygame.init()
     screen = pygame.display.set_mode((1000, 750))
     # This should show a blank 200 by 200 window centered on the screen
    #     pygame.display.flip()
    
    #     background_image=pygame.image.load("sonar.png")
     background_image=pygame.image.load("sonar1.jpg").convert_alpha() 
     screen.fill((255,255,255)) 
     screen.blit(background_image,(0,0))
    #     screen
    #     screen.fill(255)
     
     #window icon
     icon = pygame.image.load("icon.png")
     pygame.display.set_icon(icon)
     
     
     
     
     x=Button("sonar_or_radar_screen_mini_button.png",(500,600),screen)
#     print("coordenadas rectangulo botão")
#     print(x[1])
     textLabel(screen, "")
#     freeText(screen,"",500,350,(255,255,0),50,"comicsansms")
     
     pygame.display.update()
     
     array_GUI.append(screen)
     array_GUI.append(x[1])
#     print(array_GUI)   
#     print(x[1])
#     print(array_GUI[1])
  
     return array_GUI #x[1]

     
#     while running:
#         if pygame.mouse.get_pressed()[0] == True:
#            mouse_x,mouse_y=pygame.mouse.get_pos()
#            print(mouse_x)
#            print(mouse_y)
     
def textLabel(screen, text):
    # initialize font; must be called after 'pygame.init()' to avoid 'Font not Initialized' error
    myfont = pygame.font.SysFont("monospace", 50)
    
    # render text
    label = myfont.render(text, 1, (255,255,0))
    screen.blit(label, (375, 300))



def button_pressed(m_x, m_y, b_coords):
#    if pygame.mouse.get_pressed()[0] == True:
#    print("coord")
#    print(b_coords)
    
    x_left=b_coords[0]
#    print(x_left)
    y_top=b_coords[1]
#    print(y_top)
    width=b_coords[2]
    x_rigth=x_left+width
    height=b_coords[3]
    y_bot=y_top+height
    
#    mouse_x,mouse_y=pygame.mouse.get_pos()
    
#    print("m_x "+str(m_x))
#    print("m_y "+str(m_y))
#    
#    print("x_left "+str(x_left))
#    print("x_rigth "+str(x_rigth))
#    print("y_bot "+str(y_bot))
#    print("height "+str(height))
#    
#    if m_x > x_left and m_x < x_rigth:
#        print("Entrou")
#    if m_x > x_left and m_x < x_rigth:
#        print("entrou")
#    if m_y > y_top and m_y < y_bot:
#        print("entrou")
    
    if m_x > x_left and m_x < x_rigth and m_y > y_top and m_y < y_bot:
        
        print("botão foi clicado")
        return True
#        print(mouse_x)
#        print(mouse_y)
    return False
def resetLabel(screen):
    pass
    

def main():
    Serie=comInit(com,baudrate)
#    print(stringReceive(Serie))
    caracterSend(Serie,'c')
#        pygame.init()
#    GUI()
    array_GUI=GUI()
    button_coords=array_GUI[1]
#    print("coord")
#    print(button_coords)
    running = True
    
#    while s not None:
#        print stringReceive(Serie)
#        c = raw_input('Introduza caracter: ')
#        print "O caracter escolhido foi o '%c', mas podia ter sido outro\n" %c
#        caracterSend(s, c)
        
    while running:
#        if(stringReceive(Serie))
#        print(pygame.mouse.get_pressed()[0] == True)
#        print(stringReceive(Serie))
        c = input('Introduza caracter: ')
        print ("O caracter escolhido foi o '%c', mas podia ter sido outro\n" %c)
        caracterSend(Serie,'c' )
        if pygame.mouse.get_pressed()[0] == True:
                mouse_x,mouse_y=pygame.mouse.get_pos()
                if button_pressed(mouse_x, mouse_y,button_coords):
#                    stringReceive(Serie)
#                    print(GUI)
#                    print(stringReceive(Serie))
#                    reading=stringReceive(caracterSend(Serie,":1:"))
#                    print(reading)
#                    textLabel(array_GUI[0],stringReceive(Serie))
#                    freeText(array_GUI[0],stringReceive(Serie),500,350,(255,255,0),50,"comicsansms")
                    
                    pygame.display.update()
                    

        for event in pygame.event.get():
    
            if event.type == pygame.QUIT:
                print("quit")
                running = False
                pygame.quit()
        
        
        
            
            

    


# Função de inicialização
def comInit(com, baudrate):

    try:
        Serie = serial.Serial(com, baudrate)
        print ('Sucesso na ligacao ao Arduino.')
        print ('Ligado ao ' + Serie.portstr)
        return Serie
    except Exception as e:
        print ('Insucesso na ligacao ao Arduino.')
        print (e)
    return None

def caracterReceive(Serie):
    try:
        return Serie.read()
    except:
        print ('Erro na comunicacao.')
    Serie.close()
def caracterSend(Serie, info):
    
    print(info)
    try:
#        c = input('c')
#        string_encoded=info.encode("UTF-8")
#        print(string_encoded)
#        c=byte(c)
#        print(info)
#        b=b
        Serie.write(info)
#        Serie.write(c)
#        Serie.write(info)
    except:
        print ('Erro de comunicacao2.')
        Serie.close()
def stringReceive(Serie):
    try:       
        #decodes the byte type into string with the designated encoding
        string_decoded=Serie.readline().decode("UTF-8")
        
        #selects only the value of velocity from the decoded String
        space_index=string_decoded.index(" ");
        value=string_decoded[:space_index]
  
        return value
    except:
        print ('Erro na comunicacao1.')
        Serie.close()


if __name__ == '__main__':
    main()
