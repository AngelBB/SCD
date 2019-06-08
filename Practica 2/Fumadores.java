/*
* Alumno: Angel Barrilao Bensrhir
* Practica 2 - monitores
*/


import java.util.Random;
import monitor.*;

class Estanco extends AbstractMonitor{
	private Condition [] sem_estanquero;
	private Condition [] sem_fumadores = new Condition[3];
	int ingrediente_anterior;
	int id_estanquero;
	public int total_estanqueros;
	
	public Estanco(int numero_estanc){
		//Inicializamos los semaforos de los fumadores y la variable del ingrediente anterior
		this.ingrediente_anterior=-1; //Nuestro control sera -1
		this.total_estanqueros=numero_estanc;
		sem_estanquero=new Condition[total_estanqueros];
		
		for (int i=0; i<3; i++){
            		sem_fumadores[i] = makeCondition();
		}
		
		for (int i=0; i<total_estanqueros; i++){
   			sem_estanquero[i] = makeCondition();
		}
		
		
    }
	
	
	
	/*Esta funcion va ser invocada por cada fumador , 
	que este mismo le pasara el ingrediente*/
	public  void obtenerIngrediente(int ing) {
			
		enter();//bloq. Mutex
			
			/*Controlamos que el ingrediente no sea el mismo de antes (ponemos -1)
			porque no podemos hacerle dos veces seguidas await sobre un proceso que
			* todavia no ha sido liberado*/
			if(ing != ingrediente_anterior){
				sem_fumadores[ing].await();
			}	
			
			System.out.println("\033[32m" + "Fumador, fumando        " + ing + "\033[0m");
			//El fumador recoge el ingrediente y pone el ingrediente a -1			
			ingrediente_anterior=-1;
			sem_estanquero[id_estanquero].signal();//Llamamos al estanquero para que empice a producir	
						
					
		leave();//lib. mutex
		
	}
	 
	//Esta funcion la llamara el estanquero , indicandole el ingrediente que pone 
	public  void ponerIngrediente(int ingrediente,int num_estanquero){
		enter();
			if(ingrediente_anterior == -1){
				ingrediente_anterior=ingrediente;//Salvamos el ingrediente recien creado
				id_estanquero=num_estanquero;//Salvamos el estanquero
				System.out.println("\033[31m" + "Estanquero "+num_estanquero+", repartiendo " + ingrediente + "\033[0m");
				sem_fumadores[ingrediente].signal();/*Una vez creado el ingrediente llamamos al fumador
				que busca ese mismo ingrediente*/
			}	
		leave();
	}
	
	/*Esta funcion tambien va a ser invocada por el estanquero*
	 * Lo que hara, es que se mantendra en espera hasta que el ingrediente se haya consumido
	 * 
	 */
	
	public void esperarRecogidaIngrediente(int num_estanquero){
		enter();
			if(ingrediente_anterior != -1)
			/*Si el ingrediente anterior es distinto de -1 quiere decir que el fumador 
			 * todavia no ha consumido dicho ingrediente , por lo que el estanquero
			 * se queda esperando hasta que sea el ingrediente_anterior sea -1  
			 * que indicaria que el ingrediente se ha consumido
			 * */
				
				sem_estanquero[num_estanquero].await();//el estanquero espera
				
			
				
		leave();
	}
	
	//~ public int llamar_proveedor(){
		
		//~ enter();	
				//~ int ingrediente=(int)(Math.random() * 3.0);		
				//~ System.out.println("El proveedor trae: "+ingrediente);
			//~ sem_estanquero.signal();
		//~ leave();
		
		//~ return ingrediente;
	//~ }
	
	
	
}

// ****************************************************************************

class Estanquero implements Runnable{

	
	public  Thread thr;
	private Estanco est;
	int id;

	public Estanquero( Estanco es,int id_estanquero){
		est    = es;
		thr   = new Thread(this,"Estanquero "+id_estanquero);
		id = id_estanquero;
	}
	
	public void run(){
	  
		try{
			int numIngrediente;	
			int id_estanquero;
			
				while(true){
					//~ numIngrediente=est.llamar_proveedor();
					numIngrediente=(int)(Math.random() * 3.0);
					id_estanquero=(int)(Math.random() * est.total_estanqueros);
					
					est.ponerIngrediente(numIngrediente,id_estanquero);/*Ponemos el ingrediente *
					y avisamos el fumador de turno*/
					est.esperarRecogidaIngrediente(id_estanquero);/*Nos quedamos esperando hasta que 
					lo consuma el fumador*/
				}
				
		}catch( Exception e ){
			System.err.println("Excepcion en main: " + e);
		}
	}
	
	
}

// ****************************************************************************

class Fumador implements Runnable{
	
	private int ingrediente;
	public  Thread  thr;
	private Estanco est;

	public Fumador( Estanco e, int id_fum ){
		est    = e;
		ingrediente  = id_fum;
		thr   = new Thread(this,"Fumador "+id_fum);
	}
	
	public void run(){
		try{
			while(true){
				est.obtenerIngrediente(ingrediente);
				simular_fumar.dormir_max(2000);
			}
			
		}catch( Exception e ){
			System.err.println("Excepcion en main: " + e);
		}
	}
}

// ****************************************************************************


class simular_fumar{
    static Random randomize = new Random();
    
    static void dormir_max(int milisecsMax){
        try{
            Thread.sleep(randomize.nextInt(milisecsMax));
        }catch(InterruptedException e){
            System.err.println("sleep interumpido en 'aux.simular_fumar()'");
        }
    }
    
}


// ****************************************************************************

class Fumadores
{
  public static void main( String[] args )
  {
	  
	  if(args.length > 0 ){
		// leer parametros, crear vectores y buffer intermedio
        Estanco estanco = new Estanco(Integer.parseInt(args[0]));
        Estanquero[] estanquero = new Estanquero[Integer.parseInt(args[0])];
        Fumador[] fumadores = new Fumador[3];
		
		for(int i=0;i<Integer.parseInt(args[0]);i++){
			estanquero[i]=new Estanquero(estanco,i);
		}
		
		
		//Creamos las hebras	
        fumadores[0]= new Fumador(estanco, 0);
        fumadores[1]= new Fumador(estanco, 1);
        fumadores[2]= new Fumador(estanco, 2);

		for(int i=0;i<Integer.parseInt(args[0]);i++){
				//poner en marcha las hebras
				estanquero[i].thr.start();
		}
        
        fumadores[0].thr.start();
        fumadores[1].thr.start();
        fumadores[2].thr.start();
	}else{
			System.err.println("Error no has introducido el numero correcto de parametros.");
			System.err.println("Formato parametros: <numero estanqueros>");
	}
    
  }
}
