/* 
 * Archivo: funciones.c
 *
 * Este archivo contiene las funciones que se utilizan en el archivo tcpservidor
 *
 * Autores: Cristina Betancourt # Carnet 11-10104
 *  		Meggie Sanchez      # Carnet 11-10939
 */

/* Función: MaxClientes
 * Descripción: Manda un mensaje de que todos los cajeros están en uso si ya no
 *              se pueden conectar más
 * Parámetros:  
 *     - socket: descriptor del socket
 */
void MaxClientes(int socket) {

	int contador;
    char buffer[TAMBUFFER];
    
    memset(buffer, 0, TAMBUFFER);
    sprintf(buffer, "%s", "Todos los cajeros estan en uso. Por favor, intente mas tarde \n");

    if ((contador = send(socket, buffer, strlen(buffer), 0)) == -1) {
		perror(" No puedo enviar informacion \n");
		exit(-1);
	}
    
    close(socket);
}

/* Función: HoraCajero
 * Descripción: Devuelve la hora para mostrarla en el cajero de la forma HH:MM
 * Parámetros:
 *     - t: Variable necesaria para la función time
 *     - tmp: Variable para obtener la hora y los minutos
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
 *     - d: Variable necesaria para la función time
 *     - dmp: Variable para obtener la hora y los minutos
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
 * 				de todas las operaciones realizadas indicando fecha, hora, 
 * 				código del usuario, el evento (Depósito/Retiro) y Total 
 *              Disponible.
 * Parámetros:
 *    - archivo: archivo (retiro o depósito) donde se escribirá la información
 *    - fecha: fecha del cajero
 *    - hora: hora del cajero
 *    - codigo_usuario: código de usuario introducido por el cliente
 *    - monto: monto que se deposito o se retiro
 */
void EscrituraArchivo(FILE *archivo, char fecha[TAMBUFFER], 
	char hora[TAMBUFFER], int codigo_usuario, int monto, int TotalDisponible, 
	char NombreArchivo[64]) {
	
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