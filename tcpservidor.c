/* 
 * Archivo: tcpservidor.c
 *
 * Este archivo contiene 
 *
 * Autores: Cristina Betancourt # Carnet 11-10104
 *  		Meggie Sanchez      # Carnet 11-10939
 */

#include "tcpservidor.h"

int main(int argc, char *argv[]) {

	/* Fichero descriptor */
	int fp, fp2;

	/* Estructura necesaria para acceder a la información del servidor */
	struct sockaddr_in servidor;

	/* Estructura necesaria para acceder a la información del cliente */
	struct sockaddr_in cliente;

	int sin_size;

	/* Verificación para ver si el socket se creo correctamente */
	if ((fp = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  
		perror(" Error en socket() \n");
		exit(-1);
    }

    servidor.sin_family = AF_INET;         
    servidor.sin_port = htons(PUERTO); 
    servidor.sin_addr.s_addr = INADDR_ANY; 
    /* INADDR_ANY coloca nuestra dirección IP automáticamente */

    bzero(&(servidor.sin_zero), 8); 

    /* Verificación para ver si el bind se está realizando correctamente */
	if (bind(fp, (struct sockaddr*)&servidor,
		sizeof(struct sockaddr)) == -1) {
		perror(" Error en Bind() \n");
		exit(-1);
	}     

	/* Verificación para ver si el listen se está realizando correctamente */
	if (listen(fp, RESERVA) == -1) { 
		perror(" Error en Listen()\n");
		exit(-1);
	}

	while(1) {

		sin_size = sizeof(struct sockaddr_in);
		/* A continuación la llamada a accept() */
		if ((fp2 = accept(fp, (struct sockaddr *)&cliente,
			&sin_size)) == -1) {
			perror(" Error en accept() \n");
			exit(-1);
		}

		printf("Se obtuvo una conexión desde %s\n", inet_ntoa(cliente.sin_addr)); 

		send(fp2,"Bienvenido a mi servidor.\n", 25, 0); 

		close(fp2); 
	}	
}