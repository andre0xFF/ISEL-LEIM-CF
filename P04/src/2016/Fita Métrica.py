# -*- coding: utf-8 -*-
"""
Spyder Editor

This temporary script file is located here:
C:\Users\Vitor Paiva\.spyder2\.temp.py
"""

import pygame
import serial

com='COM3' 
baudrate=9600
activated = False
save = False
distancia = 0
onlaunch = False
s = None
string = None
#Funçãodeinicialização 
def comInit(com,baudrate): 
    try: 
        Serie = serial.Serial(com,baudrate) 
        print('Sucesso na ligacao ao Arduino.') 
        print('Ligado ao '+ Serie.portstr)
        return Serie
        
    except Exception as e: 
        print('Insucesso na ligacao ao Arduino.') 
        print e 
        return None
        
def caracterReceive(Serie): 
    try: 
        return Serie.read()
    except: 
        print('Erro na comunicacao.') 
        Serie.close()
def caracterSend(Serie,info):
    try: 
        Serie.write(info) 
    except: 
        print('Erro de comunicacao.') 
        Serie.close()
        
def stringReceive(Serie): 
    global string
    try: 
        string = True
                    
        return Serie.readline().strip('\r\n')
    except: 
        print('Erro na comunicacao.') 
        Serie.close() 
        
    string = False

pygame.init()

width = 1000
heigth = 600

gameWindow = pygame.display.set_mode((width, heigth))

pygame.display.set_caption("Fita Métrica")

frame_rate = 10

clock = pygame.time.Clock()

antialias = True

Grey = (250,250,250)

icon = pygame.image.load("icon.png")
pygame.display.set_icon(icon)

botao = pygame.image.load("botao2.png")
botao2 = pygame.image.load("botao3.png")
BG = pygame.image.load("BG.png")

##-------------------------------------------------------------------------------TEXTO--------------------------------------------------------------------------------

#Funções para criação de fontes centradas ao display, de modo a facilitar a escrita no display
#Texto livre, pode ser posicionado em qualquer valor de x
def freeText(text, x, y, color, size = 25, font = None):
    fontType = pygame.font.Font(font, size)
    textDisplay = fontType.render(text, antialias, color)
    textpos = textDisplay.get_rect()
    textpos.center = (x,y)
    gameWindow.blit(textDisplay, textpos)

    # Componentes adicionais, não necessárias para o funcionamento da função apenas para ajuda na composição do jogo de modo a conhecer a posição ocupada pelo texto
    rect_coordinates = [textpos.topleft, textpos.bottomright]
    return rect_coordinates


#Texto centrado à janela de jogo
def centerText(text, y, color, size = 25, font = None):
    fontType = pygame.font.Font(font, size)
    textDisplay = fontType.render(text, antialias, color)
    textpos = textDisplay.get_rect()
    textpos.center = ((width/2),y)
    gameWindow.blit(textDisplay, textpos)

    # Componentes adicionais, não necessárias para o funcionamento da função apenas para ajuda na composição do jogo de modo a conhecer a posição ocupada pelo texto
    rect_coordinates = [textpos.topleft, textpos.bottomright]
    return rect_coordinates

#Texto posicionado à direita da janela de jogo
def rightText(text, y, color, size = 25, font = None):
    fontType = pygame.font.Font(font, size)
    textDisplay = fontType.render(text, antialias, color)
    textpos = textDisplay.get_rect()
    textpos.midright = ((width-10),y)
    gameWindow.blit(textDisplay, textpos)

#Texto posicionado à esquerda da janela de jogo
def leftText(text, y, color, size = 25, font = None):
    fontType = pygame.font.Font(font, size)
    textDisplay = fontType.render(text, antialias, color)
    textpos = textDisplay.get_rect()
    textpos.midleft = (10,y)
    gameWindow.blit(textDisplay, textpos)

##-------------------------------------------------------------------------------BUTÕES--------------------------------------------------------------------------------
##x = coordenada x utilizada como ponto de referência para posicionamento da imagem
##y = posição y em que queremos colocar o canto superior esquerdo da figura/texto
##w = cumprimento da imagem/texto
##h = altura da imagem/texto
##color = cor do texto/imagem inicial
##hover = cor do texto/imagem quando o rato se encontra na posição do(a) mesmo(a)
##action = ação do botão(Predefinição None  - nenhuma ação é tomada)
##msg = mensagem que surgirá no botão (Apenas é utilizado quando não utilizamos uma imagem para o mesmo efeito)
##textType = tipo de texto, tipos de texto defenidos acima
##x_center = utilizado apenas quando textType é igual a freeText de modo a redefenir as coordenadas x em que este se encontra
##selected = selected é o que fará possivel a utilização das setas durante a escolha da dificuldade
def button(x,y,w,h,color,hover,action=None ,msg = None, textType = centerText, x_center = None, size = 15):
    global activated, onlaunch
    mouse = pygame.mouse.get_pos()
    click = pygame.mouse.get_pressed()
    if msg == None:
        if ((x-w)/2)+w > mouse[0] > (x-w)/2 and y+h > mouse[1] > y:
            gameWindow.blit(hover, (((x-w)/2),y))
            if click[0] == 1:
                if action != None:
                    activated = True
                    #action()
        else:
                gameWindow.blit(color, (((x-w)/2),y))
    else:
        if ((x-w)/2)+w > mouse[0] > (x-w)/2 and y+(h/2) > mouse[1] > y-(h/2):
            if textType == freeText:
                textType(msg, x_center, y, hover, size)
            else:
                textType(msg, y, hover, size)
            if click[0] == 1:
                if action != None:
                    action()
        else:
            if textType == freeText:
                textType(msg, x_center, y, color, size)
            else:
                textType(msg, y, color, size)
    return

def gameQuit():
    pygame.quit()
    quit()

def launch():
    global activated, onlaunch
    global distancia, s
    
    
    while (onlaunch) : 
        
        for event in pygame.event.get():
            # evento fechar a janela gráfica
            if event.type == pygame.QUIT:
                gameQuit()
                
        gameWindow.blit(BG, (0,0)) 
        #button(width, 0, 200, 200, botao, botao2, loop)
        
        pygame.display.flip()
    
        clock.tick(frame_rate)
        

def loop():
    global string
    go = False;
    s = comInit(com, baudrate)
    string2 = stringReceive(s)
    global activated, distancia
    while not(go):      
        
        gameWindow.blit(BG, (0,0)) 
        
        button(width, 300, 200, 200, botao, botao2, not(None))
        
        for event in pygame.event.get():
            # evento fechar a janela gráfica
            if event.type == pygame.QUIT:
                gameQuit()
            if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_m:
                        caracterSend(s, 'L')   
    
#       Utilizar o botão do python
        if (activated == True):
        
            caracterSend(s, 'L')
            
            activated = False
                
        if (s.inWaiting() > 0):   
            string2 = stringReceive(s)
                
        centerText(string2, 280, (255,255,255), 50)
    
        pygame.display.flip()
    
        clock.tick(10)


loop()

pygame.quit()
