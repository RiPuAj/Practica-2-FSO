// Código fuente de la librería sala
// Reto que sea multisala
#include <stdlib.h> 
#include <stdio.h>
#include "./sala.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int* listaAsientos;
int capacidadMax = 0;
int libres;
int fid;

int reserva_asiento (int id){
	
	if(libres == 0){	// Miramos si el aforo está completo
		return -1;
	}
	
	if (id < 1){					// Miramos que el id sea valido
		return -1;		// Ya que en el estado se devuelve 0 si está libre
	}
	
	int asientoLibre = -1;
	for(int i = 0; i < capacidadMax; i++){
		
		if(asientoLibre < 0){			// Buscamos un asiento libre solo 1 vez
			if(*(listaAsientos+i) == -1){
				asientoLibre = i;
			}
		}
		
		if(*(listaAsientos+i) == id){		// Miramos si la persona ya tiene un
			return -2;			// asiento asignado
		}
	}
	
	*(listaAsientos+asientoLibre) = id;		// Si no tiene asiento asignado se lo 
	libres--;					// asignamos con el guardado antes
	
	return asientoLibre;
}

int libera_asiento (int asiento){
	
	if(asiento >= capacidadMax || asiento < 0){	// Miramos que el asiento sea válido
		return -1;
	}
	
	if(*(listaAsientos+asiento) == -1){		// Miramos si el asiento ya está libre
		return -1;
	}
	
	int idActual = *(listaAsientos+asiento);	// Liberamos el asiento
	*(listaAsientos+asiento) = -1;
	libres++;
	return idActual;				// Devolvemos id de la persona sentada
}

int estado_asiento(int asiento){
	
	if(asiento >= capacidadMax || asiento < 0){	// Miramos que el asiento sea válido
		return -1;
	}
	
	if(*(listaAsientos+asiento) != -1){		// Miramos si el asiento está ocupado
		return *(listaAsientos+asiento);
	}
	
	return 0;					// Devolvemos 0 si está libre
}

int asientos_libres (){
	return libres;					// Devolvemos los asientos libres
}

int asientos_ocupados (){
	return capacidadMax-libres;		// Devolvemos los asientos ocupados
}

int capacidad (){
	return capacidadMax;				// Devolvemos la capacidad de la sala
}

void crea_sala (int capacidad){
	
	if(capacidad > 0){
	
		capacidadMax = capacidad;			
		libres = capacidadMax;
		listaAsientos = malloc(capacidadMax*sizeof(int));	// Guardamos espacio para la sala
		
		for(int i = 0; i < capacidadMax; i++){			// Inicializamos toda la sala a -1
			*(listaAsientos+i) = -1;
		}
		
		//printf("\n\x1b[34mSISTEMA\x1b[0m: Sala con capacidad %d creada correctamente\n", capacidad);
	} else {
		//printf("\x1b[31mERROR:\x1b[0m: Sala con capacidad incorrecta -> (Capacidad < 1)");
	}
}

void elimina_sala(){
	free(listaAsientos);	// Liberamos el espacio ocupado por la sala
	capacidadMax = 0;
	libres = 0;
	//printf("\x1b[34mSISTEMA\x1b[0m: Sala eliminada correctamente\n");
}

int guarda_estado_sala(const char* ruta_fichero){

	// Abrimos el fichero, en caso contrario, lo creamos
	fid = open(ruta_fichero, O_WRONLY | O_CREAT | O_TRUNC, S_IROTH | S_IWOTH |S_IRUSR| S_IWUSR);
	
	// Escribimos la capacidad
	write(fid, &capacidadMax, sizeof(int));
	
	// Escribimos los estados de los asientos
	for (int i = 0; i < capacidadMax; i++){
		int estado = estado_asiento(i);
		write(fid, &estado, sizeof(int));
	}
	
	// Cerramos el fichero
	close(fid);
	return 0;
}

int recupera_estado_sala(const char* ruta_fichero){

	// Abrimos el fichero
	if((fid =open(ruta_fichero, O_RDONLY))==-1){
		return -1;
	}
	
	// Carácter leído
	int leido;
	
	// Flag del contador
	int cap_flag = 0;
	
	// Leemos el archivo y actualizamos la sala
	while(read(fid, &leido, sizeof(int)) != 0){
		if(cap_flag == 0){
			
			// Comprobamos que no haya sala creada
			if(capacidadMax == 0){
				elimina_sala();
			}
			
			// Creamos sala
			crea_sala(leido);
			cap_flag = 1;
			
		} else {
			
			// Comprobamos que alguien está sentado y reservamos
			if(leido != 0){
				reserva_asiento(leido);
			}
		}
	}
	
	// Cerramos el fichero
	close(fid);
	return 0;
}

int guarda_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos){
	
	// Comprobamos que los asientos a guardar son posibles guardarlos
	if(capacidadMax == 0 || capacidadMax < num_asientos){
		return -1;
	}
	
	// Abrimos el fichero, en caso contrario, lo creamos
	fid = open(ruta_fichero, O_WRONLY | O_CREAT | O_TRUNC, S_IROTH | S_IWOTH |S_IRUSR| S_IWUSR);
	
	// Escribimos la capacidad del parcial de la sala (Nº asientos cogidos)
	write(fid, (int*) &num_asientos, sizeof(int));
	
	// Escribimos en el fichero los estados del asiento
	for(int i = 0; i < num_asientos; i++){
		
		int num_asi = *(id_asientos+i);
		
		// Comprobamos que el id del asiento esté 
		// dentro del intervalo de asientos
		if(num_asi > capacidadMax-1){
			close(fid);
			return -1;
		}
		
		// Escribimos el estado del asiento en el fichero
		int asiento = estado_asiento(num_asi);
		write(fid, &asiento, sizeof(int));

	}
	
	// Cerramos el fichero
	close(fid);
	return 0;
}

int recupera_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos){
	
	// Comprobamos que los asientos a guardar son posibles guardarlos
	if(capacidadMax == 0 | num_asientos > capacidadMax){
		return -1;
	}
	
	// Abrimos el fichero (Comprobamos que existe)
	if((fid =open(ruta_fichero, O_RDONLY))==-1){
		return -1;
	}
	
	int asiento;
	int estadoAsiento;
	
	for(int i = 0; i < num_asientos; i++){
		
		asiento = *(id_asientos+i);
		
		if (asiento >= capacidadMax){
			close(fid);
			return -1;
		}
		
		// Cambiamos la posición del puntero
		lseek(fid, (asiento+1)*sizeof(int), SEEK_SET);
		
		// Leemos el dato
		read(fid, &estadoAsiento, sizeof(int));

		// Actualizamos los asientos libres
		if(*(listaAsientos+asiento) == -1 && estadoAsiento != 0){
			libres--;
		}
		
		// Ponemos los asientos en el lugar indicado
		if(estadoAsiento != 0){
			*(listaAsientos+asiento) = estadoAsiento;
		}
		
	}
	return 0;	
}

