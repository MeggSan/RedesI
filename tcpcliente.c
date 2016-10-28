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

	/*Monto a depositar o retirar*/
	int monto;

	/* Ficheros descriptores */
	int fp, numbytes;

	/* Entero que representa el Puerto del servidor */
	int puerto;

	/* Caracter que representa la operacion */
	char* operacion;

	/* Entero que representa el codigo del usuario */
	int codigo_usuario;

	char buffer[TAMBUFFER];

	/* Estructura necesaria para acceder a la información del ip */
	struct hostent *ipcentral;

	/* Estructura necesaria para acceder a la información del servidor */
	struct sockaddr_in servidor;

	/* Verificacion de entrada de argumentos en el terminal */
	if (argc != 10) {
		printf(" ERROR, la entrada correcta es: bsb_cli -d <nombre_modulo_atencion> -p <puerto_bsb_svr> -c <op> -i <codigo_usuario> \n");
		exit(1);
	}
	
	if (strcmp("bsb_cli", argv[1]) != 0) {
		printf(" Entrada incorrecta: Debe comenzar con bsb_cli\n");
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
				puerto = atoi(argv[i+1]);
				if (puerto != 20104 && puerto != 20939) {
					printf("Error, el puerto debe ser 20104 o 20939 \n");
					exit(1);
				}
				break;
			
			case 'c':
				if ((strcmp("d", argv[i+1]) == 0) || (strcmp("r", argv[i+1]) == 0))
					operacion = argv[i+1];
				else {
					perror(" Error en la operacion a realizar \n");
					exit(-1);
				}
				break;
			
			case 'i':
				/* Verificacion de codigo de usuario entre el rango de enteros 1000 hasta 9999 */
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

    /* Verificación para ver si el socket se creo correctamente */
    if ((fp = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  
		perror(" Error en la funcion socket() \n");
		exit(-1);
    }

    servidor.sin_family = AF_INET;
    servidor.sin_port = puerto;
    servidor.sin_addr = *((struct in_addr *)ipcentral->h_addr); 

    bzero(&(servidor.sin_zero), 8);

    /* */
    if (connect(fp, (struct sockaddr *)&servidor,
		sizeof(struct sockaddr)) == -1) { 
		perror(" Error de connect() debido a IP incorrecto o puerto incorrecto \n");
		exit(-1);
    }
    
    /*Recibir y enviar mensajes al servidor*/
   
    /*Recibe la bienvenida*/
    if ((numbytes = recv(fp, buffer, TAMBUFFER, 0)) == -1) {  
		perror(" Error en la funcion recv() \n");
		exit(-1);
    }

    buffer[numbytes] = '\0';
    printf(" Mensaje del Servidor: %s\n", buffer); 
    
    //Se manda el tipo de operacion
 	send(fp, operacion, TAMBUFFER, 0);

    if (strcmp("r", operacion) == 0){
    	printf("Introduzca el monto a retirar:\n" );
    	char entrada[TAMBUFFER];
    	scanf("%d",&monto);
    	sprintf(entrada, "%d", monto);
    	send(fp, entrada, TAMBUFFER, 0);
    	if (monto>3000){
    		printf("El monto excede la cantidad permitida de Bs. 3000\n");
    	}
    	else{
    		
    		// Se recibe total disponible 
    		if ((numbytes = recv(fp, buffer, TAMBUFFER, 0)) == -1) {  
				perror(" Error en la funcion recv() \n");
				exit(-1);
  			}
  			buffer[numbytes] = '\0';
  			// Se verifica que el total disponible sea mayor a 5000
  			if (atoi(buffer)<=5000){
  				printf("Monto insuficiente.\n");
  			}
  			else{
  				char codigoString[TAMBUFFER];
	  			sprintf(codigoString, "%d", codigo_usuario);
  				//Se envia el codigo de usuario
  				send(fp, codigoString,TAMBUFFER,0);
  				printf("El retiro se ha realizado satisfactoriamente.\n");
  				// Se recibe la fecha 
	    		if ((numbytes = recv(fp, buffer, TAMBUFFER, 0)) == -1) {  
					perror(" Error en la funcion recv() \n");
					exit(-1);
	  			}
	  			buffer[numbytes] = '\0';
  				printf("Fecha: %s\n",buffer);
  				// Se recibe la hora 
	    		if ((numbytes = recv(fp, buffer, TAMBUFFER, 0)) == -1) {  
					perror(" Error en la funcion recv() \n");
					exit(-1);
	  			}
	  			buffer[numbytes] = '\0';
  				printf("Hora: %s\n",buffer);
  				printf("Usuario: %d\n",codigo_usuario);
			
  			}
    	}
	
    }
    else if (strcmp("d", operacion) == 0){
    	printf("Introduzca el monto a depositar:\n" );
    	char entrada[TAMBUFFER];
    	scanf("%s", entrada);
    	send(fp, entrada, TAMBUFFER, 0);
    	char codigoString[TAMBUFFER];
		sprintf(codigoString, "%d", codigo_usuario);
		//Se envia el codigo de usuario
		send(fp, codigoString,TAMBUFFER,0);
		printf("El deposito se ha realizado satisfactoriamente.\n");
		// Se recibe la fecha 
		if ((numbytes = recv(fp, buffer, TAMBUFFER, 0)) == -1) {  
			perror(" Error en la funcion recv() \n");
			exit(-1);
		}
		buffer[numbytes] = '\0';
		printf("Fecha: %s\n",buffer);
		// Se recibe la hora 
		if ((numbytes = recv(fp, buffer, TAMBUFFER, 0)) == -1) {  
			perror(" Error en la funcion recv() \n");
			exit(-1);
		}
		buffer[numbytes] = '\0';
		printf("Hora: %s\n",buffer);
		printf("Usuario: %d\n",codigo_usuario);
    }


    close(fp);
}