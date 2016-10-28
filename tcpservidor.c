/* 
 * Archivo: tcpservidor.c
 *
 * Este archivo contiene 
 *
 * Autores: Cristina Betancourt # Carnet 11-10104
 *  		Meggie Sanchez      # Carnet 11-10939
 */

 #include "tcpservidor.h"

/* Función: MaxCajeros
 * Descripción: 
 * Parámetros:
 */
 /* int MaxCajeros(int socket, struct sockaddr_in direcc) {

	int contador;
    char buffer[TAMBUFFER];
    
    memset(buffer, 0, TAMBUFFER);
    sprintf(buffer, " Este cajero no pertenece al servidor \n");

    if ((contador = send(socket, buffer, strlen(buffer), 0)) == -1) {
		perror(" No puedo enviar informacion \n");
		exit(-1);
	}
    
    close(socket);

    return 0; 
} */

/* Función: MaxClientes
 * Descripción: 
 * Parámetros:
 */
int MaxClientes(int socket, struct sockaddr_in direcc) {

	int contador;
    char buffer[TAMBUFFER];
    
    memset(buffer, 0, TAMBUFFER);
    sprintf(buffer, " Todos los cajeros estan en uso. Por favor, intente mas tarde \n");

    if ((contador = send(socket, buffer, strlen(buffer), 0)) == -1) {
		perror(" No puedo enviar informacion \n");
		exit(-1);
	}
    
    close(socket);

    return 0; 
}

/* Función: HoraCajero
 * Descripción: 
 * Parámetros:
 */
char* HoraCajero(time_t t, struct tm tmp) {

	t = time(NULL);
	tmp = *localtime(&t);

	char time[TAMBUFFER];

	int hour = tmp.tm_hour;
	int min = tmp.tm_min;
	
	sprintf(time, "%d:%d",hour,min);

	return time;
}

/* Función: FechaCajero
 * Descripción: Función que devuelve la fecha para mostrarla en el cajero
 * Parámetros:
 *     - t:
 *     - tmp:
 */
char* FechaCajero(time_t d, struct tm dmp) {

	d = time(NULL);
	dmp = *localtime(&d);

	char date[TAMBUFFER];

	int year = dmp.tm_year;
	int month = dmp.tm_mon;
	int day = dmp.tm_mday;

	sprintf(date, "%d/%d/%d",day,month,year);


	return date;
}


/* Función: EscrituraArchivo
 * Descripción: Función que escribirá en un archivo donde se llevará el registro
 * de todas las operaciones realizadas indicando fecha, hora, 
 * código del usuario, el evento (Depósito/Retiro) y Total Disponible.
 * Parámetros:
 *    - archivo:
 *    - t:
 *    - tmp:
 *    - TotalDisponible:
 */
void EscrituraArchivo(FILE *archivo, char fecha[TAMBUFFER], char hora[TAMBUFFER], int codigo_usuario, int monto, int TotalDisponible, char NombreArchivo[64]) {
	
	archivo = fopen(NombreArchivo,"a");
	fprintf(archivo, "%s %s %s ", 
					 " Fecha:", fecha, "\n");
	fprintf(archivo, "%s %s %s %s %d %s %s %d %s %s %d %s", 
					 " Hora:", hora, "\n",
					 " Codigo de usuario:", codigo_usuario, "\n",
					 " Monto en la operacion:", monto, "\n",
					 " Total Disponible:", TotalDisponible, "\n\n");

	fclose(archivo);
}


/* Función: CajeroCliente
 * Descripción: 
 * Parámetros:
 */
int CajeroCliente(int fp, time_t t, struct tm tmp, FILE *retiros, FILE *depositos, int TotalDisponible, char ArchivoDeposito[64], char ArchivoRetiro[64]) {
	
 	int monto; 
 	int codigo_usuario;
 	char* operacion;
 	char fecha[TAMBUFFER];
 	char hora[TAMBUFFER];

 	int numbytes;
	char buffer[TAMBUFFER];
	send(fp, " Bienvenido a mi servidor", 25, 0);
	
	//Se recibe el tipo de operacion (deposito o retiro)
	if ((numbytes = recv(fp, buffer, TAMBUFFER, 0)) == -1) {  
		perror(" Error en la funcion recv() \n");
		exit(-1);
  	}
  	buffer[numbytes] = '\0';
  	operacion = buffer;

  	if (strcmp("r", operacion) == 0){
  		
  		//Se recibe el monto de la operacion
		if ((numbytes = recv(fp, buffer, TAMBUFFER, 0)) == -1) {  
			perror(" Error en la funcion recv() \n");
			exit(-1);
	  	}
	  	buffer[numbytes] = '\0';
	  	monto = atoi(buffer);
	  	if (monto<=3000){
	  		char totalString[TAMBUFFER];
	  		sprintf(totalString, "%d", TotalDisponible);
	  		// Se envia el total disponible
	  		send(fp,totalString,TAMBUFFER,0);

			if (TotalDisponible>5000){
			  	//Se recibe el codigo del usuario
				if ((numbytes = recv(fp, buffer, TAMBUFFER, 0)) == -1) {  
					perror(" Error en la funcion recv() \n");
					exit(-1);
			  	}
			  	buffer[numbytes] = '\0';
			  	codigo_usuario = atoi(buffer);
			  	
			  	strcpy(fecha, FechaCajero(t, tmp));
			  	strcpy(hora, HoraCajero(t, tmp));
			  	send(fp, fecha, TAMBUFFER, 0);
				send(fp, hora, TAMBUFFER, 0);

			  	TotalDisponible -= monto;
			  	EscrituraArchivo(retiros, fecha, hora, codigo_usuario, monto, TotalDisponible, ArchivoRetiro);
			}
		}
  	}
  	else if (strcmp("d", operacion) == 0){
  		//Se recibe el monto de la operacion
		if ((numbytes = recv(fp, buffer, TAMBUFFER, 0)) == -1) {  
			perror(" Error en la funcion recv() \n");
			exit(-1);
	  	}
	  	buffer[numbytes] = '\0';
	  	monto = atoi(buffer);
	  	//Se recibe el codigo del usuario
		if ((numbytes = recv(fp, buffer, TAMBUFFER, 0)) == -1) {  
			perror(" Error en la funcion recv() \n");
			exit(-1);
	  	}
	  	buffer[numbytes] = '\0';
	  	codigo_usuario = atoi(buffer);
	  	
	  	strcpy(fecha, FechaCajero(t, tmp));
	  	strcpy(hora, HoraCajero(t, tmp));
	  	send(fp, fecha, TAMBUFFER, 0);
		send(fp, hora, TAMBUFFER, 0);

	  	TotalDisponible += monto;
	  	EscrituraArchivo(depositos, fecha, hora, codigo_usuario, monto, TotalDisponible, ArchivoDeposito);
  	}
	close(fp);
	return(TotalDisponible);

}



int main(int argc, char *argv[]) {

	/* Ficheros descriptores */
	int fp, fp2;

	/* Estructura necesaria para acceder a la información del servidor */
	struct sockaddr_in servidor;

	/* Estructura necesaria para acceder a la información del cliente */
	struct sockaddr_in cliente;

	int sin_size;

	/* Proceso id del hijo */
	int childpid;  

	/* Contador para el número de hijos */
	int countchild = 0; 

	/* Estado del proceso hijo */
	int pidstatus; 

	/* Conjunto de ficheros descriptores */
	fd_set fps;    

	/* Entero que representa el Puerto del servidor */
	int puerto;

	/* Nombre de la bitacora de deposito */
	char ArchivoDeposito[64]; 

	/* Nombre de la bitacora de retiro */
	char ArchivoRetiro[64];   

	/* Bitacora deposito (archivo) */
	FILE *depositos;

	/* Bitacora retiro (archivo) */
	FILE *retiros;

	/* Contador para la cantidad de cajeros registrados */
	/*	char* listaCajeros[3];
	listaCajeros[0] = NULL;
	listaCajeros[1] = NULL;
	listaCajeros[2] = NULL;*/

	/* Variables necesarias para la fecha y la hora del servidor */
	time_t t;
	struct tm tmp;

	/* Variable Total Disponible del servidor */
	int TotalDisponible = 80000;

	if (strcmp("bsb_svr", argv[1]) != 0) {
		printf(" Entrada incorrecta: Debe comenzar con bsb_svr\n");
		exit(1);
	}

	/* Verificacion de entrada de argumentos en el terminal */
	if (argc != 8) {
		printf(" ERROR, la entrada correcta es: bsb_svr -l <puerto_bsb_svr> -i <bitacora_deposito> -o <bitacora_retiro> \n");
		exit(1);
	}

	int i;

	for(i = 2; i < 7; i += 2) {

		switch(argv[i][1]) {
			
			case 'l':
				puerto = atoi(argv[i+1]);
				if (puerto != 20104 && puerto != 20939) {
					printf(" Error, el puerto debe ser 20104 o 20939 \n");
					exit(1);
				}
				break;
			
			case 'o':
				strcpy(ArchivoRetiro, argv[i+1]);
				retiros = fopen(ArchivoRetiro,"a");
				if (retiros == NULL) {
					printf(" Error, el nombre del archivo no debe contener caracteres especiales \n");
					exit(1);
				}
				fclose(retiros);	
				break;
			
			case 'i':
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
    servidor.sin_addr.s_addr = INADDR_ANY; 
    /* INADDR_ANY coloca nuestra dirección IP automáticamente */

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
		/* Limpia el conjunto de descriptores */
		FD_ZERO(&fps); 

		/* Se agrega el descriptor del socket al conjunto de descriptores */ 
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
				
				/* Creamos un nuevo proceso hijo */
				childpid = fork(); 

				if (childpid == -1) {
					perror(" No se pudo crear el proceso hijo \n");
					exit(-1);
				}

				else if (childpid == 0) {
/*					int listaLlena = 1;
					int j;*/
/*					for (j=0;j<3;j++) {
						if (listaCajeros[j] == NULL && listaLlena == 1) {
							listaLlena = 0;*/
/*							printf("\n Se ha conectado %s por su puerto %d\n", inet_ntoa(cliente.sin_addr), cliente.sin_port); 
							printf("TOTAL DISP ANTES: %d\n",TotalDisponible );
							TotalDisponible = CajeroCliente(fp2, t, tmp, retiros, depositos, TotalDisponible, ArchivoDeposito, ArchivoRetiro);
							printf("TOTAL DISP DESPUES: %d\n",TotalDisponible );
	
						}
					}*/

/*					if (listaLlena == 1) {
						int pertenece = 0;
						int z;
						for (z = 0; z < 3; z += 1) {
							if (inet_ntoa(cliente.sin_addr) == listaCajeros[z])
								pertenece = 1;
						}*/

/*						if (pertenece == 1) { //ip pertenece a listaCajeros*/
							
					if (MAXCLIENTES > countchild) {
						printf("\n Se ha conectado %s por su puerto %d\n", inet_ntoa(cliente.sin_addr), cliente.sin_port);
						printf(" 2 TOTAL DISP ANTES: %d\n",TotalDisponible );
						TotalDisponible = CajeroCliente(fp2, t, tmp, retiros, depositos, TotalDisponible, ArchivoDeposito, ArchivoRetiro);
						printf("2 TOTAL DISP DESPUES: %d\n",TotalDisponible );
						
					}
					else {
						exit(MaxClientes(fp2, cliente));
					}
				}
				
/*						else {
							exit(MaxCajeros(fp2, cliente)); 
						}*/
/*					}
				}*/

				else {
					countchild ++;
	/*				int x;
					for (x = 0; x < 3; x++) {
						if (listaCajeros[x] == NULL) {
							listaCajeros[x] = inet_ntoa(cliente.sin_addr);
						}
					}*/
					close(fp2); 
				}
			}


			/* NO SABEMOS COMO HACER PARA SABER CUANDO SE DESCONECTA 
			int desconectado;
			char buf[1024] = {0};
			if ((desconectado = recv(fp, buf, TAMAXBYTES, 0))==0){
				printf("Primer countchild %d\n",countchild );
				countchild --;
				printf("Segundo countchild %d\n",countchild );
			}

			childpid = waitpid(0, &pidstatus, WNOHANG);
			printf("%d\n",countchild );
			*/
		
		}

		childpid = waitpid(0, &pidstatus, WNOHANG);

		if (childpid > 0) {
/*			if (countchild > 3) 
				countchild = 2;
			else*/
			countchild --; 
		}

	}

	close(fp2);

	return 0;	
}