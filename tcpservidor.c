/* 
 * Archivo: tcpservidor.c
 *
 * Este archivo contiene 
 *
 * Autores: Cristina Betancourt # Carnet 11-10104
 *  		Meggie Sanchez      # Carnet 11-10939
 */

 #include "tcpservidor.h"
 #include "funciones.c"

/* Variables globales */
int TotalDisponible = 80000;
/* Contador para el número de hijos */
int countchild = 0; 
int idHilo = 0;
int HilosLibres[3] = {1,1,1};

// Para llevar la cuenta de cantidad de retiros hechos por los usuarios
int CantidadRetiros[8999];

// Para llevar la cuenta que no se conecte el mismo usuario más de 1 vez
int CantidadUsuarios[3];

/* Bitacora deposito (archivo) */
FILE *depositos;

/* Bitacora retiro (archivo) */
FILE *retiros;

/* Función: CajeroCliente
 * Descripción: 
 * Parámetros:
 */
void *CajeroCliente(void * atributosHilo) {

	countchild ++;

	Atributos * atributos = (Atributos *)atributosHilo;

	/* Hijos Libres */
	HilosLibres[atributos->hilo] = 0;

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

	//Se recibe el codigo del usuario
	if ((numbytes = recv(atributos->fp, buffer, TAMBUFFER, 0)) == -1) {
		perror(" 3 Error en la funcion recv() \n");
		exit(-1);
  	}
  	buffer[numbytes] = '\0';
  	codigo_usuario = atoi(buffer);

  	int i;
  	int conectado = 0;
  	for (i = 0; i < 3; i++) {
  		if (CantidadUsuarios[i] == codigo_usuario) {
  			conectado = 1;
  		}
  	}
  	if (conectado == 1) {
  		send(atributos->fp, "Usuario conectado", 17, 0);
  	}
  	else {
  		send(atributos->fp, "Usuario disponible", 18, 0);
  		for (i = 0; i < 3; i++){
  			if (CantidadUsuarios[i] == 0) {
  				CantidadUsuarios[i] = codigo_usuario;
  				break;
  			}
  		}
  		send(atributos->fp, " Bienvenido a mi servidor", 25, 0);
		
		//Se recibe el tipo de operacion (deposito o retiro)
		if ((numbytes = recv(atributos->fp, buffer, TAMBUFFER, 0)) == -1) {  
			perror(" 1 Error en la funcion recv() \n");
			exit(-1);
	  	}
	  	buffer[numbytes] = '\0';
	  	operacion = buffer;

	  	if (strcmp("r", operacion) == 0) {
	  		
			
		  	if(CantidadRetiros[codigo_usuario -1000]==3){
		  		send(atributos->fp,"DENEGADO",8,0);
		  	}
		  	else if (CantidadRetiros[codigo_usuario -1000] < 3){
		  		send(atributos->fp,"PERMITIDO",9,0);
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
					  	
					  	strcpy(fecha, FechaCajero(t, tmp));
					  	strcpy(hora, HoraCajero(t, tmp));
					  	send(atributos->fp, fecha, TAMBUFFER, 0);
						send(atributos->fp, hora, TAMBUFFER, 0);

					  	TotalDisponible -= monto;
					  	EscrituraArchivo(retiros, fecha, hora, codigo_usuario, monto, TotalDisponible, atributos->ArchivoRetiro);
						CantidadRetiros[codigo_usuario -1000] ++;
					}
				}
			}
	  	}

	  	else if (strcmp("d", operacion) == 0) {
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
	  	for (i = 0; i < 3; i++){
  			if (CantidadUsuarios[i] == codigo_usuario) {
  				CantidadUsuarios[i] = 0;
  				break;
  			}
  		}
  	}
	//close(atributos->fp);
	HilosLibres[atributos->hilo] = 1;
	free(atributos);
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

	int i;

	for (i = 0; i < 8999; i++) {
		CantidadRetiros[i] = 0;
	}

	int j;

	for (j = 0; j < 3; j++) {
		CantidadUsuarios[j] = 0;
	}

	if (strcmp("bsb_svr", argv[1]) != 0) {
		printf(" Entrada incorrecta: Debe comenzar con bsb_svr\n");
		exit(1);
	}

	/* Verificacion de entrada de argumentos en el terminal */
	if (argc != 8) {
		printf(" ERROR, la entrada correcta es: bsb_svr -l <puerto_bsb_svr> -i <bitacora_deposito> -o <bitacora_retiro> \n");
		exit(1);
	}

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
					valor_hilo = pthread_create(&h[idHilo], NULL, CajeroCliente, (void *)atributosHilo);
					if (valor_hilo != 0) {
						perror("NO SE PUDO CREAR HILO");
						exit(0);
					}
					printf(" TOTAL DISP ANTES: %d\n",TotalDisponible );
				}
				else {
					MaxClientes(fp2, cliente);
				}
				 
			}
		}
	}

	printf("\n Se ha desconectado %s por su puerto %d\n", inet_ntoa(cliente.sin_addr), cliente.sin_port);
	close(fp2);

	return 0;	
}