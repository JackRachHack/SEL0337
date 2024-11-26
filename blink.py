from gpiozero import LED # importa biblio para lidar com o LED
from time import sleep # importa biblio para gerar timer

pin = LED(24) # define pino 24 como saida do LED

while True: # laco de repeticao infinita
	pin.off() # desliga o LED
	sleep(0.2) # espera 0.2 segundos
	
	pin.on() # liga o LED
	sleep(0.2) # espera 0.2 segundos
	
