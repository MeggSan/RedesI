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
#include <pthread.h>

/* Constantes necesarias */

#define TAMBUFFER 512 /* Tamaño del buffer */
#define MAXCLIENTES 3 /* Conexiones permitidas */

/* Renombrando Estructura Atributos a Atributos. */
typedef struct Atributos Atributos;

/* Estructura de Datos Atributos para los hilos */
struct Atributos {
	int fp; 
	char *ArchivoRetiro; 
	char *ArchivoDeposito;
	int hilo;

};

/* Declaración de funciones */

/* Procedimiento MaxClientes para ver si todos los cajeros están en uso */
void MaxClientes(int socket);

/* Función que devuelve la hora para el cajero */
char* HoraCajero(time_t t, struct tm tmp);

/* Función que devuelve la fecha para el cajero */
char* FechaCajero(time_t t, struct tm tmp);

/* Procedimiento que permite la escritura de información de las operaciones de 
   los clientes */
void EscrituraArchivo(FILE *archivo, char fecha[TAMBUFFER], char hora[TAMBUFFER], 
	int codigo_usuario, int monto, int TotalDisponible, char NombreArchivo[64]);