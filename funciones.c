/* 
 * Archivo: funciones.c
 *
 * Este archivo contiene 
 *
 * Autores: Cristina Betancourt # Carnet 11-10104
 *  		Meggie Sanchez      # Carnet 11-10939
 */

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