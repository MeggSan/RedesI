 # 
 # Archivo: Makefile
 #
 #
 # Autores: Cristina Betancourt # Carnet 11-10104
 #  		Meggie Sanchez      # Carnet 11-10939
 #
 # Makefile para aplicación de un sistema de redes para cajeros automáticos

CC = gcc
CFLAGS = -g

all : tcpservidor tcpcliente

tcpservidor : tcpservidor.c
	$(CC) $(CFLAGS) -o tcpservidor tcpservidor.c -lpthread

tcpcliente : tcpcliente.c 
	$(CC) $(CFLAGS) -o tcpcliente tcpcliente.c

clean :
	/bin/rm tcpservidor tcpcliente

