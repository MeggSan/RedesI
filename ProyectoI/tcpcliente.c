/* 
 * Archivo: tcpcliente.c
 *
 * Este archivo contiene todo lo referente al cliente, verificación de la 
 * entrada en el terminal, conexión que se realiza a través del socket con el 
 * servidor, y el intercambio de mensajes necesarios entre el cliente y servidor
 *
 * Autores: Cristina Betancourt # Carnet 11-10104
 *  		Meggie Sanchez      # Carnet 11-10939
 */

#include "tcpcliente.h"

int main(int argc, char *argv[]) {

	/* Monto a depositar o retirar */
	int monto;

	/* Ficheros descriptores */
	int fp, numbytes;

	/* Entero que representa el Puerto del servidor */
	int puerto;

	/* Caracter que representa la operación */
	char* operacion;

	/* Entero que representa el código del usuario */
	int codigo_usuario;

	/* Buffer necesario que se usa para guardar los mensajes del servidor */
	char buffer[TAMBUFFER];

	/* Estructura necesaria para acceder a la información del ip */
	struct hostent *ipcentral;

	/* Estructura necesaria para acceder a la información del servidor */
	struct sockaddr_in servidor;

	/*********************************************************************/
	/*                  VERIFICACIONES PARA EL TERMINAL                  */
	/*********************************************************************/

	/* Verificación de entrada de argumentos en el terminal */
	if (argc != 10) {
		printf(" ERROR, la entrada correcta es: bsb_cli -d <nombre_modulo_atencion> -p <puerto_bsb_svr> -c <op> -i <codigo_usuario> \n");
		exit(1);
	}
	
	if (strcmp("bsb_cli", argv[1]) != 0) {
		printf(" Entrada incorrecta: Debe comenzar con bsb_cli \n");
		exit(1);
	}
	
	int i;
	for(i = 2; i < 9; i += 2) {
		
		switch(argv[i][1]) {

			case 'd':
				/* Verificación del IP en el argumento correspondiente */
				if ((ipcentral = gethostbyname(argv[i+1])) == NULL) {       
					perror(" Error en la funcion gethostbyname() \n");
					exit(-1);
			    }
			    break;
			
			case 'p':
				/* Verificación del puerto en el argumento correspondiente y que
				   sea el puerto correcto */
				puerto = atoi(argv[i+1]);
				if (puerto != 20104 && puerto != 20939) {
					printf(" Error, el puerto debe ser 20104 o 20939 \n");
					exit(1);
				}
				break;
			
			case 'c':
				/* Verificación de la operación a realizar en el argumento 
				   correspondiente que sea únicamente d y r */
				if ((strcmp("d", argv[i+1]) == 0) || (strcmp("r", argv[i+1]) == 0)) {
					operacion = argv[i+1];
				}
				else {
					perror(" Error en la operacion a realizar \n");
					exit(-1);
				}
				break;
			
			case 'i':
				/* Verificación de código de usuario entre el rango de enteros 
				   1000 hasta 9999 */
				if (atoi(argv[i+1]) > 999 && atoi(argv[i+1]) < 10000)  
					codigo_usuario = atoi(argv[i+1]);

				else {
					perror(" El codigo de usuario no se encuentra registrado\n");
					exit(-1);
				}
				break;

			default:
				printf(" Entrada incorrecta\n");
				exit(1);
		}
	}

	/*********************************************************************/
	/*                               SOCKET                              */
	/*********************************************************************/

    /* Verificación para ver si el socket se creó correctamente */
    if ((fp = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  
		perror(" Error en la funcion socket() \n");
		exit(-1);
    }

    servidor.sin_family = AF_INET;
    servidor.sin_port = puerto;
    servidor.sin_addr = *((struct in_addr *)ipcentral->h_addr); 

    bzero(&(servidor.sin_zero), 8);

    /* Verificación para ver si el connect se realizo correctamente */
    if (connect(fp, (struct sockaddr *)&servidor,
		sizeof(struct sockaddr)) == -1) { 
		perror(" Error de connect() debido a IP incorrecto o puerto incorrecto \n");
		exit(-1);
    }
	/*********************************************************************/
	/*        INTERCAMBIOS DE MENSAJES ENTRE EL SERVIDOR Y CLIENTE       */
	/*********************************************************************/

    /* Envia el código de usuario */
  	char codigoString[16];
	sprintf(codigoString, "%d", codigo_usuario);
	send(fp, codigoString, 4, 0);

	/* Recibe si ya esta conectado un usuario con Usuario Disponible o Usuario
	   Conectado */
    if ((numbytes = recv(fp, buffer, 19, 0)) == -1) {  
		perror(" Error en la funcion recv() \n");
		exit(-1);
    }
    buffer[numbytes] = '\0';

    if (strcmp(buffer, " Usuario conectado ") == 0) {
		printf(" Ya se encuentra conectado este usuario \n");
	}

	else if (strcmp(buffer, " Usuario disponible") == 0) {

	    /* Recibe la bienvenida del servidor */
	    if ((numbytes = recv(fp, buffer, 51, 0)) == -1) {  
			perror(" Error en la funcion recv() \n");
			exit(-1);
	    }
	    buffer[numbytes] = '\0';

	    printf(" Mensaje del Servidor: %s\n", buffer); 

	    if (strcmp(buffer, "Todos los cajeros estan en uso. Intente mas tarde \n") == 0) {
			exit(0);
	    }

	    else {
	    
		    /* Envia la operación a realizar */
		 	send(fp, operacion, 1, 0);

		    if (strcmp("r", operacion) == 0) {

				/* Recibe si se puede realizar el retiro con Denegado o 
				   Permitido */
				if ((numbytes = recv(fp, buffer, 10, 0)) == -1) {  
					perror(" Error en la funcion recv() \n");
					exit(-1);
	    		}
	    		buffer[numbytes] = '\0';

	    		if (strcmp(buffer, " Denegado") == 0) {
	    			printf(" Ya se ha excedido de la cantidad de veces de retiro \n");
	    		}

	    		else if (strcmp(buffer, " Permitido") == 0) {

	    			/* Se espera para introducir el monto que se va a retirar */
	    			printf(" Introduzca el monto a retirar: \n");

			    	char entrada[TAMBUFFER];
			    	scanf("%d", &monto);
			    	sprintf(entrada, "%d", monto);

			    	/* Envia el monto a retirar al servidor */
			    	send(fp, entrada, TAMBUFFER, 0);

			    	if (monto > 3000) {
			    		printf(" El monto excede la cantidad permitida de Bs. 3000 \n");
			    	}

			    	else {
			    		
			    		/* Recibe el total disponible */
			    		if ((numbytes = recv(fp, buffer, TAMBUFFER, 0)) == -1) {  
							perror(" Error en la funcion recv() \n");
							exit(-1);
			  			}
			  			buffer[numbytes] = '\0';

			  			/* Verifica que el total disponible sea mayor a 5000 */
			  			if (atoi(buffer) <= 5000) {
			  				printf(" Dinero No disponible. \n");
			  			}

			  			else {

			  				printf(" El retiro se ha realizado satisfactoriamente.\n");

			  				/* Recibe la fecha */ 
				    		if ((numbytes = recv(fp, buffer, 10, 0)) == -1) { 
								perror(" Error en la funcion recv() \n");
								exit(-1);
				  			}
				  			buffer[numbytes] = '\0';
			  				printf(" Fecha: %s\n", buffer);

			  				/* Recibe la hora */
				    		if ((numbytes = recv(fp, buffer, 5, 0)) == -1) {
								perror(" Error en la funcion recv() \n");
								exit(-1);
				  			}
				  			buffer[numbytes] = '\0';
			  				printf(" Hora: %s\n", buffer);

			  				printf(" Usuario: %d\n", codigo_usuario);
			  			}
			    	}
	    		}
		    }

		    else if (strcmp("d", operacion) == 0) {

		    	/* Se espera para introducir el monto que se va a depositar */
		    	printf(" Introduzca el monto a depositar: \n");

		    	char entrada[TAMBUFFER];
		    	scanf("%s", entrada);

		    	/* Envia el monto a depositar al servidor */
		    	send(fp, entrada, TAMBUFFER, 0);

		    	char codigoString[16];
				sprintf(codigoString, "%d", codigo_usuario);

				/* Envia el código de usuario al servidor */
				send(fp, codigoString, 16, 0);
				printf(" El deposito se ha realizado satisfactoriamente. \n");

				/* Recibe la fecha */
				if ((numbytes = recv(fp, buffer, 10, 0)) == -1) {
					perror(" Error en la funcion recv() \n");
					exit(-1);
				}
				buffer[numbytes] = '\0';
				printf(" Fecha: %s\n", buffer);

				/* Recibe la hora */
				if ((numbytes = recv(fp, buffer, 5, 0)) == -1) {
					perror(" Error en la funcion recv() \n");
					exit(-1);
				}
				buffer[numbytes] = '\0';
				printf(" Hora: %s\n", buffer);

				printf(" Codigo de usuario: %d\n", codigo_usuario);
		    }
		}
	}

    close(fp);
}