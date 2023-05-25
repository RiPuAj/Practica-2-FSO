// Programa de copia de archivos
// ./copia -o fichero origen -d fichero_destino [-y]

#include <stdio.h>	// fprintf / printf
#include <stdlib.h>	// para exit
#include <string.h>	// Para strlen
#include <getopt.h>
#include <string.h>
#include "../cabeceras/sala.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char* nombre_archivo;
int f_flag = 0, o_flag = 0, c_flag = 0, n_flag = 0, a_flag = 0, i_flag = 0;
int c_argumento, n_argumento, a_argumento, i_argumento;
int fid_misala;
int cero = 0;

void lanza_error(int error){
	switch(error){
		
		// Errores de sintaxis
		case 0:
			fprintf(stderr, "\x1b[31mError de sintaxix\x1b[0m Uso: ./misala crea -f [-o] fichero origen -c capacidad\n");
			break;
			
		case 1:
			fprintf(stderr, "\x1b[31mError de sintaxix\x1b[0m Uso: ./misala reserva -f fichero origen -n Num Asientos id1 id2...\n");
			break;
		
		case 2:
			fprintf(stderr, "\x1b[31mError de sintaxix\x1b[0m Uso: ./misala anula -f fichero origen [-a][-i] id1 id2 ...\n");
			break;
		
		case 3:
			fprintf(stderr, "\x1b[31mError de sintaxix\x1b[0m Uso: ./misala estado -f fichero origen\n");
			break;
			
		case 4:
			fprintf(stderr, "\x1b[31mError de apertura:\x1b[0m Permisos insuficientes o archivo inexistente.\n");
			break;
			
		case 5:
			fprintf(stderr, "\x1b[31mError:\x1b[0m Operación imposible. Sala llenada\n");
			break;
		
		case 6:
			fprintf(stderr, "\x1b[31mError:\x1b[0m Archivo ya existente. -o para sobreescribir (./misala help +info).\n");
			break;
				
		default:
			fprintf(stderr, "\x1b[31mError:\x1b[0m Comando erróneo. ./misala help para ayuda.\n");
			break;
	}
	
	exit(1);
}

int comprobar_array(int elemento_array, int* lista_asientos, int len_lista){
	
	for(int i = 0; i < len_lista; i++){
		if(lista_asientos[i] == elemento_array){
			return 1;
		}
	}
	
	return 0;
}

void estado_sala(char* archivo){
	
	// Función para printear el estado de la sala (Nº Asientos e IDs)
	
	printf("\n\x1b[32mRESULTADO:\x1b[0m SALA GRÁFICA\n");
	printf("\nNº -> Número de asiento\n");
	printf("ES -> Estado del asiento:\n");
	printf("\t0 -> Asiento libre\n");
	printf("\tID > 0 -> Asiento ocupado por ese ID\n");
	printf("Nº");

	int fid = open(archivo, O_RDONLY);
	int capFile;	
	int iter = 0;
	int estado_asiento;
	
	read(fid, &capFile, sizeof(int));
	
	for(int i = 0; i < capFile; i++){
		printf("|%5.1d", i);
		if ((i+1) % 10 == 0){
			printf("\nID");
		
			for(int j = i-10+1; j<i+1; j++){
				read(fid, &estado_asiento, sizeof(int));
				printf("|%5.1d", estado_asiento);
			}
			
			if(i+1 < capFile){
				printf("\nNº");
				iter = i+1;
			}else{
				printf("\n");
				continue;
			}
			
		}
		
		if(i == capFile-1){
			printf("\nID");
			int n = capFile;
			for(int j = iter; j<n; j++){
				read(fid, &estado_asiento, sizeof(int));
				printf("|%5.1d", estado_asiento);
			}
			printf("\n");
		}
	}
}

void extraer_modificadores(int argc, char* argv[], const char* modAceptados){
	int param;
	
	while((param = getopt(argc, argv, modAceptados)) != -1){
		switch(param){
			case 'f':
				nombre_archivo = malloc(sizeof(char)*strlen(optarg));
				strcpy(nombre_archivo, optarg);
				f_flag = 1;
				break;
				
			case 'o':
				o_flag = 1;
				break;
			
			case 'c':
				c_argumento = atoi(optarg);
				c_flag = 1;
				break;
			
			case 'n':
				n_argumento = atoi(optarg);
				n_flag = 1;
				break;
				
			case 'a':
				a_flag = 1;
				break;
				
			case 'i':
				i_flag = 1;
				break;
		}
	}
}

void main(int argc, char* argv[]){
	char* comando = argv[1];
	
	if(strcmp(comando, "crea") == 0){
	
		// Extraemos modificadores
		extraer_modificadores(argc, argv, ":f:c:o");
		
		// Comprobamos modificadores
		if(f_flag != 1 || c_flag != 1){
			
			lanza_error(0); 
	
		}
		
		// Comprobamos el modificador opcional
		if(o_flag != 1)	{
		
			// Abrimos fichero
			fid_misala = open(nombre_archivo, O_RDWR);
		
			// Comprobamos de que existe
			if (fid_misala != -1) lanza_error(6);
		}
		
		// Abrimos fichero y creamos o sobreescribimos
		fid_misala = open(nombre_archivo, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		
		// Escribimos la capacidad
		write(fid_misala, &c_argumento, sizeof(int));
		
		// Rellenamos la sala con 0's
		for(int i = 0; i < c_argumento; i++){
		
			write(fid_misala, &cero, sizeof(int));
		
		}
		
		// Cerramos el archivo
		close(fid_misala);
		exit(0);


	} else if (strcmp(comando, "reserva") == 0){
		
		// Reserva de asientos
		
		// Buscamos modificadores
		extraer_modificadores(argc, argv, ":f:n:");
		
		// Miramos que se han puesto los modificadores obligatorios (-f y -n)
		if(f_flag != 1 | n_flag != 1){
			
			lanza_error(1); // ERROR DE SINTAXIS
		}
		
		// Abrimos el fichero
		fid_misala = open(nombre_archivo, O_RDWR);
		
		// Comprobamos que el fichero ha sido creado
		if(fid_misala == -1){
			lanza_error(1); // ERROR FICHERO NO CREADO
		}
		
		// Comprobamos que el numero de ids es el mismo que el número de reservas
		int args;
		if(((args = argc - 6)) != n_argumento){
			lanza_error(1); // FALTAN ARGUMENTOS / SINTAXIS
		}
		
		// Comprobamos que el número de asientos es mayor que 0
		if(n_argumento < 1){
			
			lanza_error(1); // ERROR DE SINTAXIS
		}
			
		// Miramos si hay asientos libres
		recupera_estado_sala(nombre_archivo);				
		
		if(asientos_libres() < n_argumento){
			
			lanza_error(5); // NO HAY ASIENTOS LIBRES
	
		}
		
		elimina_sala();
		
		
		// Reservamos asientos
		int leido;
		int contador = 0;
		int nEscritos = 0;
		int indArgv = 6;
		
		while (read(fid_misala, &leido, sizeof(int)) != 0){

			// Comprobamos que hemos añadido todos los elementos
			if (nEscritos == n_argumento){
				break;
			}
			
			int elemento = atoi(argv[indArgv]);			
			
			// Reservamos asiento
			if(leido == 0){
				
				lseek(fid_misala, contador*sizeof(int), SEEK_SET);
				write(fid_misala, &elemento, sizeof(int));
				nEscritos++;
				indArgv++;
			}
			
			contador++;
		}
		
		close(fid_misala);
		exit(0);
			
			
	} else if (strcmp(comando, "anula") == 0){
		
		// Extraemos modificadores
		extraer_modificadores(argc, argv, ":f:ai");
		
		// Comprobamos que los modificadores están bien puestos
		if(f_flag != 1 || (a_flag == 1 && i_flag == 1) || (a_flag == 0 && i_flag == 0)){

			lanza_error(2);	// ERROR DE SINTAXIS
		}
		
		fid_misala = open(nombre_archivo, O_RDWR);
		
		// No se pueden abrir el archivo
		if(fid_misala == -1) {
		
			lanza_error(4); // ERROR PERMISOS
		}
		
		// Comprobamos que se haya puesto el -a		
		if(a_flag == 1){

			// Anulamos asiento 
			for(int i = 5; i < argc; i++){
				
				lseek(fid_misala, (atoi(argv[i])+1)*sizeof(int), SEEK_SET);
				write(fid_misala, &cero, sizeof(int));
			
			}

		} else if (i_flag == 1) {
		
			// Creamos lista con los elementos a eliminar
			int* lista_anular = malloc(sizeof(int)*(argc-5));
			
			for(int i = 0; i < argc-5; i++){

				lista_anular[i] = atoi(argv[5+i]);

			}
			
			int cap;
			int leido;
			int estado;
			
			// Leemos la capacidad
			read(fid_misala, &cap, sizeof(int));
			
			// Leemos y sobreescribimos el fichero
			for(int i = 1; i < cap+1; i++){
				
				// Leemos los elementos
				read(fid_misala, &leido, sizeof(int));
				
				// Comprobamos si los números pasados están en la sala
				estado = comprobar_array(leido, lista_anular, argc-5);
				
				if(estado == 1){
					
					// Cambiamos el puntero y escribimos
					lseek(fid_misala, i*sizeof(int), SEEK_SET);
					write(fid_misala, &cero, sizeof(int));
				}
			
			}
			
			
		}
		
		close(fid_misala);
		exit(0);
		
	} else if (strcmp(comando, "estado") == 0){
		
		// Extraemos los modificadores
		extraer_modificadores(argc, argv, ":f:");
		
		// Comprobamos que esté puesto el -f
		if(f_flag != 1){
			
			lanza_error(2);
		}
		
		// Enseñamos el estado de la sala
		estado_sala(nombre_archivo);
		exit(0);

	} else if (strcmp(comando, "compara") == 0){
	
		char* nombre_archivo1 = argv[2];
		char* nombre_archivo2 = argv[3];
		
		int fid_1 = open(nombre_archivo1, O_RDONLY);
		int fid_2 = open(nombre_archivo2, O_RDONLY);
		
		// Si los archivos no se pueden abrir
		if(fid_1 == -1 || fid_2 == -1){
			
			lanza_error(4);
		
		}
		
		int cap_1;
		int cap_2;
		
		read(fid_1, &cap_1, sizeof(int));
		read(fid_2, &cap_2, sizeof(int));
		
		// Comprobamos que las capacidades sean iguales
		if(cap_1 != cap_2){
			fprintf(stderr, "Los archivos son diferentes.\n\n");
			exit(1);
		}
		
		int leido_1;
		int leido_2;
		
		for(int i = 0; i < cap_1; i++){
			
			// Leemos los elementos
			read(fid_1, &leido_1, sizeof(int));
			read(fid_2, &leido_2, sizeof(int));
			
			// Comprobamos si son distintos
			if(leido_1 != leido_2){
			
				fprintf(stderr, "\x1b[32mRESULTADO:\x1b[0m Los archivos son diferentes.\n\n");
				exit(1);
			
			}
		}
		
		fprintf(stderr, "\x1b[32mRESULTADO:\x1b[0m Los archivos son iguales\n\n");		
		exit(0);
	
	
	}else if (strcmp(comando, "help") == 0){

		printf("Listado de comandos:\n");
		printf("\tcrea\t -> Sintaxis: ./misala -f[o] fichero_origen -c capacidad\n");
		printf("\treserva\t -> Sintaxis: ./misala -f fichero_origen -n Num_Asientos id1 id2 ...\n");	
		printf("\tanula\t -> Sintaxis: ./misala anula -f fichero_origen -a id1 id2 ...\n");	
		printf("\testado\t -> Sintaxis: ./misala estado -f fichero_origen\n");
		exit(0);
	}
	
	lanza_error(404); // Comando no encontrado
}



