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

/* Variables globales */
int TotalDisponible = 80000;
/* Contador para el número de hijos */
int countchild = 0; 
int idHilo = 0;
int HilosLibres[3] = {1,1,1};
pthread_mutex_t lock;      
pthread_mutex_t lock1;      
pthread_mutex_t lock2; 
pthread_mutex_t lock3;
/* Bitacora deposito (archivo) */
FILE *depositos;

/* Bitacora retiro (archivo) */
FILE *retiros;


/* Función: MaxClientes
 * Descripción: 
 * Parámetros:
 */
void MaxClientes(int socket, struct sockaddr_in direcc) {

	int contador;
    char buffer[TAMBUFFER];
    
    memset(buffer, 0, TAMBUFFER);
    sprintf(buffer, " Todos los cajeros estan en uso. Por favor, intente mas tarde \n");

    if ((contador = send(socket, buffer, strlen(buffer), 0)) == -1) {
		perror(" No puedo enviar informacion \n");
		exit(-1);
	}
    
    close(socket);
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
void *CajeroCliente(void * atributosHilo) {
	countchild ++;

	Atributos * atributos = (Atributos *)atributosHilo;

	/* Variables necesarias para la fecha y la hora del servidor */
	time_t t;
	struct tm tmp;
	
 	int monto; 
 	int codigo_usuario;
 	char* operacion;
 	char fecha[TAMBUFFER];
 	char hora[TAMBUFFER];

 	int numbytes;
	char buffer[TAMBUFFER];
	send(atributos->fp, " Bienvenido a mi servidor", 25, 0);
	
	//Se recibe el tipo de operacion (deposito o retiro)
	if ((numbytes = recv(atributos->fp, buffer, TAMBUFFER, 0)) == -1) {  
		perror(" 1 Error en la funcion recv() \n");
		exit(-1);
  	}
  	buffer[numbytes] = '\0';
  	operacion = buffer;

  	if (strcmp("r", operacion) == 0){
  		
  		//Se recibe el monto de la operacion
		if ((numbytes = recv(atributos->fp, buffer, TAMBUFFER, 0)) == -1) { 
			perror(" 2 Error en la funcion recv() \n");
			exit(-1);
	  	}
	  	buffer[numbytes] = '\0';
	  	monto = atoi(buffer);
	  	if (monto<=3000){
	  		char totalString[TAMBUFFER];
	  		sprintf(totalString, "%d", TotalDisponible);
	  		// Se envia el total disponible
	  		send(atributos->fp,totalString,TAMBUFFER,0);

			if (TotalDisponible>5000){
			  	//Se recibe el codigo del usuario
				if ((numbytes = recv(atributos->fp, buffer, TAMBUFFER, 0)) == -1) {
					perror(" 3 Error en la funcion recv() \n");
					exit(-1);
			  	}
			  	buffer[numbytes] = '\0';
			  	codigo_usuario = atoi(buffer);
			  	
			  	strcpy(fecha, FechaCajero(t, tmp));
			  	strcpy(hora, HoraCajero(t, tmp));
			  	send(atributos->fp, fecha, TAMBUFFER, 0);
				send(atributos->fp, hora, TAMBUFFER, 0);

			  	TotalDisponible -= monto;
			  	EscrituraArchivo(retiros, fecha, hora, codigo_usuario, monto, TotalDisponible, atributos->ArchivoRetiro);
			}
		}
  	}
  	else if (strcmp("d", operacion) == 0){
  		//Se recibe el monto de la operacion
		if ((numbytes = recv(atributos->fp, buffer, TAMBUFFER, 0)) == -1) { 
			perror(" 4 Error en la funcion recv() \n");
			exit(-1);
	  	}
	  	buffer[numbytes] = '\0';
	  	monto = atoi(buffer);
	  	//Se recibe el codigo del usuario
		if ((numbytes = recv(atributos->fp, buffer, TAMBUFFER, 0)) == -1) {  
			perror(" 5 Error en la funcion recv() \n");
			exit(-1);
	  	}
	  	buffer[numbytes] = '\0';
	  	codigo_usuario = atoi(buffer);
	  	
	  	strcpy(fecha, FechaCajero(t, tmp));
	  	strcpy(hora, HoraCajero(t, tmp));
	  	send(atributos->fp, fecha, TAMBUFFER, 0);
		send(atributos->fp, hora, TAMBUFFER, 0);

	  	TotalDisponible += monto;
	  	EscrituraArchivo(depositos, fecha, hora, codigo_usuario, monto, TotalDisponible, atributos->ArchivoDeposito);
  	}
	close(atributos->fp);
	countchild --;
}



int main(int argc, char *argv[]) {

	/************************* HILOS *************************/

	pthread_t h[3];
	int valor_hilo;

	/* Ficheros descriptores */
	int fp, fp2;

	/* Estructura necesaria para acceder a la información del servidor */
	struct sockaddr_in servidor;

	/* Estructura necesaria para acceder a la información del cliente */
	struct sockaddr_in cliente;

	int sin_size;

	/* Proceso id del hijo */
	int childpid;  

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

	

	/* Contador para la cantidad de cajeros registrados */
	/*	char* listaCajeros[3];
	listaCajeros[0] = NULL;
	listaCajeros[1] = NULL;
	listaCajeros[2] = NULL;*/

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

	/************************* HILOS *************************/

	while(1) {
		
        // Busca el primer hilo que se encuentre libre.
        if (HilosLibres[0] == 1 ){
            idHilo = 0;
        }
        else if (HilosLibres[1] == 1 ){
            idHilo = 1;
        }
        else if (HilosLibres[2] == 1 ){
            idHilo = 2;
        }
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
				Atributos *atributosHilo = (Atributos *)malloc(sizeof(Atributos));
				atributosHilo->fp = fp2;
				atributosHilo->hilo = idHilo;
				atributosHilo->ArchivoDeposito = ArchivoDeposito;
				atributosHilo->ArchivoRetiro = ArchivoRetiro;

				if (MAXCLIENTES > countchild) {
					printf("\n Se ha conectado %s por su puerto %d\n", inet_ntoa(cliente.sin_addr), cliente.sin_port);
					printf(" 2 TOTAL DISP ANTES: %d\n",TotalDisponible );
					//countchild ++;
					valor_hilo = pthread_create(&h[idHilo], NULL, CajeroCliente, (void *)atributosHilo);
					if (valor_hilo != 0) {
						perror("NO SE PUDO CREAR HILO");
						exit(0);
					}
					printf("2 TOTAL DISP DESPUES: %d\n",TotalDisponible );
					//countchild --;
				}
				else {
					MaxClientes(fp2, cliente);
				}
				 
			}
		}
	}

	close(fp2);

	return 0;	
}