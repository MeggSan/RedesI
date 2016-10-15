/* 
 * Archivo: tcpservidor.h
 *
 * Este archivo contiene las declaraciones de las funciones necesarias para 
 * la implementación del problema planteado en el proyecto. Posee además todas
 * las librerías y constantes necesarias.
 *
 * Autores: Cristina Betancourt # Carnet 11-10104
 *  		Meggie Sanchez      # Carnet 11-10939
 */

/* Librerías utilizadas */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>    
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* Constantes necesarias */

#define PUERTO 3550 /* Puerto */
#define TAMBUFFER 512 /* Tamaño del buffer */
#define RESERVA 3 /* Conexiones permitidas */
#define MAXCAJEROS 3 /* Conexiones permitidas */

/* Declaración de funciones */

int MaxClientes(int socket, struct sockaddr_in direcc);
int CajeroCliente(int socket, struct sockaddr_in direcc);