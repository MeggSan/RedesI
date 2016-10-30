/* 
 * Archivo: tcpservidor.c
 *
 * Este archivo contiene todo lo referente al servidor, verificación de la 
 * entrada en el terminal, conexión que se realiza a través del socket con el 
 * cliente, y el intercambio de mensajes necesarios entre el servidor y cliente
 *
 * Autores: Cristina Betancourt # Carnet 11-10104
 *  		Meggie Sanchez      # Carnet 11-10939
 */

 #include "tcpservidor.h"
 #include "funciones.c"

	/*********************************************************************/
	/*                           VARIABLES GLOBALES                      */
	/*********************************************************************/

int TotalDisponible = 80000;

/* Contador para el número de hijos */
int countchild = 0; 

/* Identificador hilo */
int idHilo = 0;

/* Arreglo que indica si el hilo esta disponible o no */
int HilosLibres[3] = {1,1,1};

/* Para llevar la cuenta de cantidad de retiros hechos por los usuarios */
int CantidadRetiros[8999];

/* Para llevar la cuenta que no se conecte el mismo usuario más de 1 vez */
int CantidadUsuarios[3];

/* Bitacora deposito (archivo) */
FILE *depositos;

/* Bitacora retiro (archivo) */
FILE *retiros;

	/*********************************************************************/
	/*        INTERCAMBIOS DE MENSAJES ENTRE EL SERVIDOR Y CLIENTE       */
	/*********************************************************************/

/* Procedimiento: CajeroCliente
 * Descripción: Este procedimiento es el que llama cada hilo al momento de 
 * 				crearse el mismo, y se encarga de lo referente al intercambio 
 				de mensajes entre cada cajero y el servidor
 */
void *CajeroCliente(void * atributosHilo) {

	countchild ++;

	Atributos * atributos = (Atributos *)atributosHilo;

	/* Hilos Libres */
	HilosLibres[atributos->hilo] = 0;

	/* Variables necesarias para la fecha y la hora del servidor */
	time_t t;
	struct tm tmp;
	
	/* Declaraciones de variables para el monto, codigo de usuario, operación,
	   fecha y hora */
 	int monto; 
 	int codigo_usuario;
 	char* operacion;
 	char fecha[8];
 	char hora[5];

 	int numbytes;
	char buffer[TAMBUFFER];

	/* Recibe el código del usuario */
	if ((numbytes = recv(atributos->fp, buffer, 4, 0)) == -1) {
		perror(" 3 Error en la funcion recv() \n");
		exit(-1);
  	}
  	buffer[numbytes] = '\0';
  	codigo_usuario = atoi(buffer);

  	/* Verificación para ver si algún usuario ya esta conectado */
  	int i;
  	int conectado = 0;
  	for (i = 0; i < 3; i++) {
  		if (CantidadUsuarios[i] == codigo_usuario) {
  			conectado = 1;
  		}
  	}
  	if (conectado == 1) {
  		printf("IF Conectado %d \n",conectado);
  		/* Envia si el usuario ya esta conectado */
  		send(atributos->fp, " Usuario conectado ", 19, 0);
  	}
  	else {
  		/* Envia si el usuario no esta conectado */
  		send(atributos->fp, " Usuario disponible", 19, 0);
  		for (i = 0; i < 3; i++) {
  			if (CantidadUsuarios[i] == 0) {
  				CantidadUsuarios[i] = codigo_usuario;
  				break;
  			}
  		}

  		/* Envia la bienvenida al cliente  */
  		send(atributos->fp, " Bienvenido a mi servidor", 25, 0);

		/* Recibe el tipo de operación (depósito o retiro) */
		if ((numbytes = recv(atributos->fp, buffer, 1, 0)) == -1) {  
			perror(" 1 Error en la funcion recv() \n");
			exit(-1);
	  	}
	  	buffer[numbytes] = '\0';
	  	operacion = buffer;


	  	if (strcmp("r", operacion) == 0) {
	  		
		  	if (CantidadRetiros[codigo_usuario - 1000] == 3) {

		  		/* Envia el denegado al cliente ya que se puede retirar max 
		  		   3 veces */
		  		send(atributos->fp, " Denegado", 9, 0);
		  	}

		  	else if (CantidadRetiros[codigo_usuario -1000] < 3) {

		  		/* Envia el permitido al cliente de que puede seguir retirando */
		  		send(atributos->fp, " Permitido", 10, 0);

		  		/* Recibe el monto del retiro */
				if ((numbytes = recv(atributos->fp, buffer, TAMBUFFER, 0)) == -1) { 
					perror(" 2 Error en la funcion recv() \n");
					exit(-1);
			  	}
			  	buffer[numbytes] = '\0';

			  	monto = atoi(buffer);

			  	if (monto <= 3000) {
			  		char totalString[TAMBUFFER];
			  		sprintf(totalString, "%d", TotalDisponible);

			  		/* Envia el total disponible */
			  		send(atributos->fp, totalString, TAMBUFFER, 0);

					if (TotalDisponible > 5000) {
					  	
					  	strcpy(fecha, FechaCajero(t, tmp));
					  	strcpy(hora, HoraCajero(t, tmp));

					  	/* Envia la fecha */
					  	send(atributos->fp, fecha, 10, 0);

						/* Envia la hora */
						send(atributos->fp, hora, 5, 0);

					  	TotalDisponible -= monto;
					  	EscrituraArchivo(retiros, fecha, hora, codigo_usuario, monto, TotalDisponible, atributos->ArchivoRetiro);
						CantidadRetiros[codigo_usuario - 1000] ++;
					}
				}
			}
	  	}

	  	else if (strcmp("d", operacion) == 0) {

	  		/* Recibe el monto del depósito */
			if ((numbytes = recv(atributos->fp, buffer, TAMBUFFER, 0)) == -1) { 
				perror(" 4 Error en la funcion recv() \n");
				exit(-1);
		  	}
		  	buffer[numbytes] = '\0';

		  	monto = atoi(buffer);
		  	/* Recibe el código del usuario */
			if ((numbytes = recv(atributos->fp, buffer, 16, 0)) == -1) {  
				perror(" 5 Error en la funcion recv() \n");
				exit(-1);
		  	}
		  	buffer[numbytes] = '\0';

		  	codigo_usuario = atoi(buffer);
		  	
		  	strcpy(fecha, FechaCajero(t, tmp));
		  	strcpy(hora, HoraCajero(t, tmp));

		  	/* Envia la fecha */
		  	send(atributos->fp, fecha, 10, 0);

		  	/* Envia la hora */
			send(atributos->fp, hora, 5, 0);

		  	TotalDisponible += monto;
		  	EscrituraArchivo(depositos, fecha, hora, codigo_usuario, monto, TotalDisponible, atributos->ArchivoDeposito);
	  	}

	  	for (i = 0; i < 3; i++) {
  			if (CantidadUsuarios[i] == codigo_usuario) {
  				CantidadUsuarios[i] = 0;
  				break;
  			}
  		}
  	}


	HilosLibres[atributos->hilo] = 1;
	free(atributos);
	countchild --;
}

int main(int argc, char *argv[]) {

	/* Declaración de los hilos */
	pthread_t h[3];
	int valor_hilo;

	/* Ficheros descriptores */
	int fp, fp2;

	/* Estructura necesaria para acceder a la información del servidor */
	struct sockaddr_in servidor;

	/* Estructura necesaria para acceder a la información del cliente */
	struct sockaddr_in cliente;

	/* Tamaño requerido para el socket */
	int sin_size;

	/* Conjunto de ficheros descriptores */
	fd_set fps;    

	/* Entero que representa el Puerto del servidor */
	int puerto;

	/* Nombre de la bitacora de deposito */
	char ArchivoDeposito[64]; 

	/* Nombre de la bitacora de retiro */
	char ArchivoRetiro[64];   

	int i;

	for (i = 0; i < 8999; i++) {
		CantidadRetiros[i] = 0;
	}

	int j;

	for (j = 0; j < 3; j++) {
		CantidadUsuarios[j] = 0;
	}

	/*********************************************************************/
	/*                  VERIFICACIONES PARA EL TERMINAL                  */
	/*********************************************************************/

	if (strcmp("bsb_svr", argv[1]) != 0) {
		printf(" Entrada incorrecta: Debe comenzar con bsb_svr\n");
		exit(1);
	}

	/* Verificación de entrada de argumentos en el terminal */
	if (argc != 8) {
		printf(" ERROR, la entrada correcta es: bsb_svr -l <puerto_bsb_svr> -i <bitacora_deposito> -o <bitacora_retiro> \n");
		exit(1);
	}

	for(i = 2; i < 7; i += 2) {

		switch(argv[i][1]) {
			
			case 'l':
				/* Verificación del puerto en el argumento correspondiente y que
				   sea el puerto correcto */
				puerto = atoi(argv[i+1]);
				if (puerto != 20104 && puerto != 20939) {
					printf(" Error, el puerto debe ser 20104 o 20939 \n");
					exit(1);
				}
				break;
			
			case 'o':
				/* Verificación para que se abra y cree correctamente el archivo
				   retiro */
				strcpy(ArchivoRetiro, argv[i+1]);
				retiros = fopen(ArchivoRetiro,"a");
				if (retiros == NULL) {
					printf(" Error, el nombre del archivo no debe contener caracteres especiales \n");
					exit(1);
				}
				fclose(retiros);	
				break;
			
			case 'i':
				/* Verificación para que se abra y cree correctamente el archivo
				   depósito */
				strcpy(ArchivoDeposito, argv[i+1]);
				depositos = fopen(ArchivoDeposito,"a");
				if (depositos == NULL) {
					printf(" Error, el nombre del archivo no debe contener caracteres especiales \n");
					exit(1);
				}
				fclose(depositos);
				break;

			default:
				printf(" Entrada incorrecta\n");
				exit(1);
		}
	}

	/* Verificación para ver si el socket se creo correctamente */
	if ((fp = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  
		perror(" Error en socket() \n");
		exit(-1);
    }

    servidor.sin_family = AF_INET;         
    servidor.sin_port = puerto; 

    /* INADDR_ANY coloca nuestra dirección IP automáticamente */
    servidor.sin_addr.s_addr = INADDR_ANY; 
    
    bzero(&(servidor.sin_zero), 8); 

    /* Verificación para ver si el bind se está realizando correctamente */
	if (bind(fp, (struct sockaddr*)&servidor,
		sizeof(struct sockaddr)) == -1) {
		perror(" Error en Bind()\n");
		exit(-1);
	}     

	/* Verificación para ver si el listen se está realizando correctamente */
	if (listen(fp, MAXCLIENTES) == -1) { 
		perror(" Error en Listen()\n");
		exit(-1);
	}

	while(1) {
		
        /* Busca el primer hilo que se encuentre libre */
        if (HilosLibres[0] == 1) {
            idHilo = 0;
        }

        else if (HilosLibres[1] == 1) {
            idHilo = 1;
        }

        else if (HilosLibres[2] == 1) {
            idHilo = 2;
        }

		/* Limpia el conjunto de descriptores */
		FD_ZERO(&fps); 

		/* Agrega el descriptor del socket al conjunto de descriptores */ 
    	FD_SET(fp, &fps); 

		sin_size = sizeof(struct sockaddr_in);

		if (select(fp+1, &fps, NULL, NULL, NULL)) {

			/* A continuación la llamada a accept() */
			if ((fp2 = accept(fp, (struct sockaddr *)&cliente,
				&sin_size)) == -1) {
				perror(" Error en accept() \n");
				exit(-1);
			}

			else {

				/* Atributos del hilo */
				Atributos *atributosHilo = (Atributos *)malloc(sizeof(Atributos));
				atributosHilo->fp = fp2;
				atributosHilo->hilo = idHilo;
				atributosHilo->ArchivoDeposito = ArchivoDeposito;
				atributosHilo->ArchivoRetiro = ArchivoRetiro;

				/* Para verificar que no hayan más de 3 conexiones */
				if (MAXCLIENTES > countchild) {
					printf("\n Se ha conectado %s por su puerto %d\n", inet_ntoa(cliente.sin_addr), cliente.sin_port);
					valor_hilo = pthread_create(&h[idHilo], NULL, CajeroCliente, (void *)atributosHilo);
					if (valor_hilo != 0) {
						perror(" No se pudo crear el hilo");
						exit(0);
					}
				}
				
				else {
					MaxClientes(fp2);
				}
				 
			}
		}
	}

	printf("\n Se ha desconectado %s por su puerto %d\n", inet_ntoa(cliente.sin_addr), cliente.sin_port);
	close(fp2);

	return 0;	
}