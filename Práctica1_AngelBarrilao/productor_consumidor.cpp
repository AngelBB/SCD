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
 * @brief Simula la produccion y consumicion sincronizada de datos en un buffer
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


using namespace std ;

/**
 * @brief Declaramos las variables tipo <const unsigned>
 * para num_items y tam_vector. 
 * Declaramos los semaforos , un buffer de tipo <vector> 
 * y una varible de tipo <int> para la ocupacion del buffer.
 *  
 * */


// ---------------------------------------------------------------------
// constantes configurables:
const unsigned
  num_items  = 40 ,    // numero total de items que se producen o consumen
  tam_vector = 10 ;    // tamaño del vector, debe ser menor que el número de items

sem_t producir,consumir,mutex; //Declaramos los semaforos
vector<int>buffer(tam_vector);
int ocupacion=0;
  



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
 * @brief Función que simula la producción de un dato
 * @return Devuelve una cifra
 * @retval unsigned
 * 
 * 
 * */

// ---------------------------------------------------------------------
// función que simula la producción de un dato

unsigned producir_dato()
{
  static int contador = 0 ;
  contador = contador + 1 ;
  retraso_aleatorio( 0.1, 0.5 );
  cout << "Productor : dato producido: " << contador << endl << flush ;
  return contador ;
}
// ---------------------------------------------------------------------
// función que simula la consumición de un dato




/**
 * @brief Función que simula la consumicion de un dato
 * @param dato Paramero de entrada que se mostrara en pantalla
 * 
 * 
 * */
void consumir_dato( int dato )
{
   retraso_aleatorio( 0.1, 1.5 );
   cout << "Consumidor: dato consumido: " << dato << endl << flush ;
}
// ---------------------------------------------------------------------

/**
 * @brief Función que ejecuta la hebra del productor
 * @return NULL
 * @param ptr tipo void
 * 
 * 
 * */
// función que ejecuta la hebra del productor

void * funcion_productor( void * )
{
  for( unsigned i = 0 ; i < num_items ; i++ )
  {
    int dato = producir_dato() ;
	
	sem_wait(&producir);
	sem_wait(&mutex);
		buffer[ocupacion]=dato;
		ocupacion++;
	sem_post(&mutex);
	sem_post(&consumir);
	
    
    cout << "Productor : dato insertado: " << dato << endl << flush ;
   
    
  }
  return NULL ;
}
// ---------------------------------------------------------------------
// función que ejecuta la hebra del consumidor

/**
 * @brief Función que ejecuta la hebra del consumidor
 * @return NULL
 * @param ptr tipo void
 * 
 * 
 * */

void * funcion_consumidor( void * )
{
  for( unsigned i = 0 ; i < num_items ; i++ )
  {
    int dato ;
	
	sem_wait(&consumir);
	sem_wait(&mutex);
		dato=buffer[ocupacion-1];
		ocupacion--;
	sem_post(&mutex);
	sem_post(&producir);
	
	
    cout << "Consumidor:  dato extraído : " << dato << endl << flush ;
    
    consumir_dato( dato ) ;
  }
  return NULL ;
}
//----------------------------------------------------------------------

int main()
{
	
	//Inicializamos los semaforos
	sem_init(&producir,0,tam_vector);
	sem_init(&consumir,0,0);
	sem_init(&mutex,0,1);
	
	//Creamos las hebras
	pthread_t consumidora,productora;
	
	pthread_create(&productora,NULL,funcion_productor,(void *)1);
	pthread_create(&consumidora,NULL,funcion_consumidor,(void *)2);
	
	pthread_join(productora,NULL);
	pthread_join(consumidora,NULL);
	
	
	//Destruimos los semaforos
	
	sem_destroy(&producir);
	sem_destroy(&consumir);
	sem_destroy(&mutex);
	
	cout<<"FIN"<<endl;

   return 0 ;
}
