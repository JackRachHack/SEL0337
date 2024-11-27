#!/bin/bash

echo 24 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio24/direction

while [ 1 ]
	do
		echo 1 > /sys/class/gpio/gpio24/value
		sleep 0.2s
		echo 0 > /sys/class/gpio/gpio24/value
		sleep 0.2s
	done
#Troque a porta por 18. Vc pode usar /sys/class/gpio/unexport para liberar o
#uso do recurso. Tente tambem reverter a atualizacao do firmware e refazer.
#Tente tambem pular o arquivo bash e ir direto pro python. Vc deve chamar o
#interpretador do python ao inves do do bash usando #!/bin/python
