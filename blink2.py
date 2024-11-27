from gpiozero import LED # importa biblio para lidar com o LED
from time import sleep # importa lib para contar tempo

pin = LED(23) # define o LED verde para o pino 23, como saida

contador = 0 # variavel para contar os ciclos
while contador < 10: # laco de repeticao finita
	pin.on() # liga o LED
	sleep(0.2) # espera 0.2 segundos
	
	pin.off() # desliga o LED
	sleep(0.2) # espera 0.2 segundo
	contador += 1 # incrementa o contador para contar as iteracoes
