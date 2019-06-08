// *****************************************************************************
//
// Prácticas de SCD. Práctica 1.
// Plantilla de código para el ejercicio del productor-consumidor con
// buffer intermedio.
//
// *****************************************************************************



/**
 * 
 * @file
 * @brief Simula a un estanquero y sus fumadores
 * @author Angel Barrilao
 * 
 * */


#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // necesario para {\ttbf usleep()}
#include <stdlib.h> // necesario para {\ttbf random()}, {\ttbf srandom()}
#include <time.h>   // necesario para {\ttbf time()}

using namespace std;
/**
 * @brief Declaramos los semaforos tipo <sem_t>
 * para cada uno de los actores del problema.
 *  
 * */

//Semaforos
sem_t fumadores[3];
sem_t estanquero; 
sem_t mutex;

/**
 * @brief introduce un retraso aleatorio de duración comprendida entre
 * 'smin' y 'smax' (dados en segundos)
 * @param smin minimo de segundos
 * @param smax maximo de segundos
 * 
 * 
 * */
// ---------------------------------------------------------------------
// introduce un retraso aleatorio de duración comprendida entre
// 'smin' y 'smax' (dados en segundos)

void retraso_aleatorio( const float smin, const float smax )
{
  static bool primera = true ;
  if ( primera )        // si es la primera vez:
  {  srand(time(NULL)); //   inicializar la semilla del generador
     primera = false ;  //   no repetir la inicialización
  }
  // calcular un número de segundos aleatorio, entre {\ttbf smin} y {\ttbf smax}
  const float tsec = smin+(smax-smin)*((float)random()/(float)RAND_MAX);
  // dormir la hebra (los segundos se pasan a microsegundos, multiplicándos por 1 millón)
  usleep( (useconds_t) (tsec*1000000.0)  );
}

/**
 * @brief Función que simula la accion de fumar. Mostrara por pantalla el fumador
 * que esta fumando.
 * @param Recibe el id de un fumador como entero
 **/

void fumar(int num_fum){
	cout << "Fumador numero "<<num_fum<<": comienza a fumar."<<endl;
	retraso_aleatorio(0.2 ,0.8);
	cout<<"Fumador numero "<<num_fum<<": termina de fumar"<<endl;
}

/**
 * @brief Función que ejecuta la hebra del estanquero
 * @param ptr tipo void
 **/
void * funcion_estanquero(void *){
	while (true){
		/*incluye el cero pero excluye el maximo(3) 
		es decir llega hasta 2.*/
		unsigned int ingrediente_aleatorio=(rand() % 3 + 0);//Producimos ingr. aleatorio
		
		sem_wait(&estanquero);//Para la prox. estanquero esta parado
		sem_wait(&mutex);//Bloquemos secc. critica
				cout<<"Estanquero genera: "<<ingrediente_aleatorio<<endl;
		sem_post(&mutex);//Desbloqueamos secc. critica
		sem_post(&fumadores[ingrediente_aleatorio]);/*Avisamos a fumador 
		segun el ingrediente aleatorio generado  */
			
	}
}

/**
 * @brief Función que ejecuta la hebra del fumador
 * @param ptr tipo void, indica el ID del fumador
 **/
void * funcion_fumador(void* num_fum_void){
	unsigned long ih= (unsigned long) num_fum_void;
	
	while (true){
		sem_wait(&fumadores[ih]);//Bloquemos el fumador especificado
			//*************VACIO*****************
		sem_post(&estanquero);//avisamos al estanquero para que empieze a producir
				fumar(ih);//Empieza a fumar
	}
}


int main(){
	srand(time(NULL));//Inizializa la semilla aleatoria
	
	
	//Inicializamos las hebras
	int  n_hebras=4;// numero de hebras
	pthread_t hebras[n_hebras];//array de hebras
	
	
	//Inicializaos los semaforos
	sem_init(&estanquero,0,1);//El estanquero empiza produciendo
	
	for(int i=0;i<n_hebras-1;i++){
		sem_init(&fumadores[i],0,0);
	}
	
	sem_init(&mutex,0,1);//La exclusion de la zon. critica empieza entrando
	
	
	//Creamos las hebras DE FUMADORES
	for (unsigned int  i=0;i<n_hebras-1;i++){
		pthread_create(&hebras[i],NULL,funcion_fumador,(void*)i);
	}
	//Creo aparte la hebra del estanquero
	pthread_create(&hebras[3],NULL,funcion_estanquero,(void*)3);
	
	
	//Lanzamos las hebras
	for(int i=0;i<n_hebras;i++){
		pthread_join(hebras[i],NULL);
	}
	
	//Destruimos los semaforos
	sem_destroy(&estanquero);
	
	for(int i=0;i<n_hebras-1;i++){
		sem_destroy(&fumadores[i]);
	}
	
	sem_destroy(&mutex);
	
	
	
	
	
	
	
}



