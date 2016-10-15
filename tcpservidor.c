/* 
 * Archivo: tcpservidor.c
 *
 * Este archivo contiene 
 *
 * Autores: Cristina Betancourt # Carnet 11-10104
 *  		Meggie Sanchez      # Carnet 11-10939
 */

#include "tcpservidor.h"

int MaxClientes(int socket, struct sockaddr_in direcc) {

	int contador;
    char buffer[TAMBUFFER];
    
    memset(buffer, 0, TAMBUFFER);
    sprintf(buffer, " Todos los cajeros estan en uso. Por favor, intente mas tarde \n");

    if ((contador = send(socket, buffer, strlen(buffer), 0)) == -1) {
		perror(" No puedo enviar información \n");
		exit(-1);
	}
    
    close(socket);

    return 0; 
}

int CajeroCliente(int socket, struct sockaddr_in direcc) {

	while(1){

		if(0>1)
			printf("Es mentira\n");
	}
	close(socket);
	return(0);

}


int main(int argc, char *argv[]) {

	/* Ficheros descriptores */
	int fp, fp2;

	/* Estructura necesaria para acceder a la información del servidor */
	struct sockaddr_in servidor;

	/* Estructura necesaria para acceder a la información del cliente */
	struct sockaddr_in cliente;

	int sin_size;
	int childpid;   /* Proceso id del hijo */
	int countchild; /* Contador para el número de hijos */
	int pidstatus;  /* Estado del proceso hijo */

	fd_set fps;        /* Conjunto de ficheros descriptores */

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

		FD_ZERO(&fps); /* Limpia el conjunto de descriptores */
    	FD_SET(fp, &fps); /* Se agrega el descriptor del socket al 
    					     conjunto de descriptores */ 

		sin_size = sizeof(struct sockaddr_in);

		if (select(fp+1, &fps, NULL, NULL, NULL)) {

			/* A continuación la llamada a accept() */
			if ((fp2 = accept(fp, (struct sockaddr *)&cliente,
				&sin_size)) == -1) {
				perror(" Error en accept() \n");
				exit(-1);
			}
			else {
				printf("\n Se ha conectado %s por su puerto %d\n", inet_ntoa(cliente.sin_addr), cliente.sin_port);
				
				childpid = fork(); /* Creamos un nuevo proceso hijo */
				if (childpid == -1) {
					perror(" No se pudo crear el proceso hijo \n");
					exit(-1);
				}

				else if (childpid == 0) {
					if (MAXCAJEROS > countchild)
						exit(CajeroCliente(fp2, cliente));
					else
						exit(MaxClientes(fp2, cliente));
				}

				else {
					countchild ++;
					close(fp2);
				}
			}
		}

		childpid = waitpid(0, &pidstatus, WNOHANG);

		if (childpid > 0) {
			countchild --;
		}

		send(fp2, " Bienvenido a mi servidor.\n", 25, 0); 
	}

	close(fp2);

	return 0;	
}