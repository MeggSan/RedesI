/* 
 * Archivo: tcpcliente.c
 *
 * Este archivo contiene 
 *
 * Autores: Cristina Betancourt # Carnet 11-10104
 *  		Meggie Sanchez      # Carnet 11-10939
 */

#include "tcpcliente.h"

int main(int argc, char *argv[]) {

	/* Fichero descriptor */
	int fp;

	char buffer[TAMAXBYTES];

	/* Estructura necesaria para acceder a la información del ip */
	struct hostent *ipcentral;

	/* Estructura necesaria para acceder a la información del servidor */
	struct sockaddr_in servidor;

	/* Verificación del IP en el argumento correspondiente */
	if ((ipcentral = gethostbyname(argv[1])) == NULL) {       
		perror(" Error en la funcion gethostbyname() \n");
		exit(-1);
    }

    /* Verificación de cantidad de argumentos explícitos en el terminal */
    if (argc != 2) { 
		printf(" No se ha colocado el numero de argumentos correctos");
		exit(-1);
    }

    /* Verificación para ver si el socket se creo correctamente */
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  
		perror(" Error en la funcion socket() \n");
		exit(-1);
    }

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);
    servidor.sin_addr = *((struct in_addr *)ipcentral->h_addr); 



    printf(" Mensaje del Servidor: %s\n", buf); 

    close(fp);
}