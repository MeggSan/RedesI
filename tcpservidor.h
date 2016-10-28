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
#include <time.h>

/* Constantes necesarias */

#define TAMBUFFER 512 /* Tamaño del buffer */
#define MAXCLIENTES 3 /* Conexiones permitidas */

/* Declaración de funciones */

int MaxCajeros(int socket, struct sockaddr_in direcc);
int MaxClientes(int socket, struct sockaddr_in direcc);
char* HoraCajero(time_t t, struct tm tmp);
char* FechaCajero(time_t t, struct tm tmp);
void EscrituraArchivo(FILE *archivo, char fecha[TAMBUFFER], char hora[TAMBUFFER], int codigo_usuario, int monto, int TotalDisponible, char NombreArchivo[64]);
int CajeroCliente(int fp, time_t t, struct tm tmp, FILE *retiros, FILE *depositos,int TotalDisponible, char ArchivoDeposito[64], char ArchivoRetiro[64]);
