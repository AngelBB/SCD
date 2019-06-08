#include <iostream>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"
#include <mpi.h>

using namespace std;

//Atributos de control
#define soltar 	0
#define coger 	1



// Lista de colores para consola formato ANSI
#define BLACK    "\033[0m"
#define RED      "\033[31m"
#define GREEN    "\033[32m"
#define YELLOW   "\033[33m"
#define BLUE     "\033[34m"
#define MAGENTA  "\033[35m"
#define CYAN     "\033[36m"
#define WHITE    "\033[37m"



// ---------------------------------------------------------------------

void Filosofo( int id, int nprocesos )
{
   int izq = (id+1) % nprocesos;
   int der = ((id+nprocesos)-1) % nprocesos;   
   
   
   while(1)
   {
     
    //El primer filosofo tiene que coger los tenedores al reves para que no se produzca interbloqueo 
    if(id == 0)
    {
		//Solicita tenedor derecho
		cout << GREEN << "Filosofo "<<id<< " coge tenedor der ..." << der << BLACK << endl << flush;
		MPI_Ssend(NULL, 0, MPI_INT, der, coger, MPI_COMM_WORLD);
		
		//solicita tenedor izquierdo
		cout<< RED <<"Filosofo "<<id<< " solicita tenedor izq ..." << izq << BLACK << endl << flush;
		MPI_Ssend(NULL, 0, MPI_INT, izq, coger, MPI_COMM_WORLD);
	}else//resto de filosofos 
	{
		//solicita tenedor izquierdo
		cout << RED <<"Filosofo "<<id<< " solicita tenedor izq ..." << izq << BLACK << endl << flush;
		MPI_Ssend(NULL, 0, MPI_INT, izq, coger, MPI_COMM_WORLD);
	
		//Solicita tenedor derecho
		cout<< GREEN << "Filosofo "<<id<< " coge tenedor der ..." << der << BLACK << endl << flush;
		MPI_Ssend(NULL, 0, MPI_INT, der, coger, MPI_COMM_WORLD);
	}
     
      cout<<"Filosofo "<<id<< " COMIENDO" << BLACK << endl <<flush;
      sleep((rand() % 3)+1);  //comiendo
      
      //Suelta el tenedor izquierdo
      cout << RED <<  "Filosofo "<<id<< " suelta tenedor izq ..." << izq << BLACK << endl << flush;
      MPI_Ssend(NULL, 0, MPI_INT, izq, soltar, MPI_COMM_WORLD);
      
      //Suelta el tenedor derecho
      cout << GREEN << "Filosofo "<<id<< " suelta tenedor der ..." << der << BLACK << endl << flush;
      MPI_Ssend(NULL, 0, MPI_INT, der, soltar, MPI_COMM_WORLD);
      
      //Piensa (espera bloqueada aleatorio del proceso)
      cout << "Filosofo " << id << " PENSANDO" << BLACK << endl << flush;
      
      //espera bloqueado durante un intervalo de tiempo aleatorio 
      //(entre una décima de segundo y un segundo)
      usleep( 1000U * (100U+(rand()%900U)) );
 }
}
// ---------------------------------------------------------------------

void Tenedor(int id, int nprocesos)
{
  int buf; 
  MPI_Status status; 
  int Filo;
  
  while( true )
  {
    //Espera una peticion desde cualquier filosofo vecino
    MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, coger, MPI_COMM_WORLD, &status);
	
	//Recibe la peticion del filosofo ...
	Filo=status.MPI_SOURCE;//Obtenemos el filosofo que nos invoca
    cout << BLUE << "Ten. " << id << " recibe petic. de " << Filo << BLACK << endl << flush;
    
    //Espera a que el filosofo suelte el tenedor...
    MPI_Recv(&Filo,1,MPI_INT,Filo,soltar,MPI_COMM_WORLD,&status);
    cout << BLUE << "Ten. " << id << " recibe liberac. de " << Filo << BLACK << endl << flush; 
  }
}
 

// ---------------------------------------------------------------------
int main( int argc, char** argv )
{
   int rank, size;
   
   srand(time(0));
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   MPI_Comm_size( MPI_COMM_WORLD, &size );
   
   
   if( size!=10)
   {
      if( rank == 0) 
         cout<<"El numero de procesos debe ser 10" << endl << flush ;
      MPI_Finalize( ); 
      return 0; 
   }
   
   if ((rank%2) == 0)  
      Filosofo(rank,size); // Los pares son Filosofos 
   else 
      Tenedor(rank,size);  // Los impares son Tenedores 
   
   MPI_Finalize( );
   return 0;
}    
