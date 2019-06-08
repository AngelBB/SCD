/*
* Alumno: Angel Barrilao Bensrhir
* Practica 2 - monitores
*/

import java.util.Random;
import monitor.*;



class Barberia extends AbstractMonitor{ 

	private Condition sem_barbero	=	makeCondition();
	private Condition sem_silla 	=	makeCondition();
	private Condition sem_cliente 	=	makeCondition();




	// invocado por los clientes para cortarse el pelo
	public void cortarPelo(){
		enter();
			//Si la silla esta ocupada
			if(!sem_silla.isEmpty()){
				System.out.println("\033[34m"+"Silla ocupada " + "\033[0m");
				sem_cliente.await();//Cliente se espera
			}
			
			// Pelamos al cliente (despertamos al barbero y ponemos al cliente en la silla)
            System.out.println("\033[32m"+"Cliente empieza a afeitarse " + "\033[0m");
			sem_barbero.signal();//Llamamos al barbero
			sem_silla.await();//La silla se queda en espera (la estamos usando)
			
		leave();
	}

	// invocado por el barbero para esperar (si procede) a un nuevo cliente y sentarlo para el corte
	public void siguienteCliente(){
		enter();
			
			// Si la sala(clientes) y la silla están vacias...
			if(sem_cliente.isEmpty() && sem_silla.isEmpty()){
				 // ...ponemos al barbero a dormir
                System.out.println("\033[31m"+"Barbero se pone a dormir " + "\033[0m");
                sem_barbero.await();
			}
			
			
			//Sacamos al siguiente cliente de la sala de espera
			System.out.println("\033[32m"+"Barbero coge otro cliente " + "\033[0m");
			sem_cliente.signal();//Liberamos al cliente
			
		leave();
	}

	// invocado por el barbero para indicar que ha terminado de cortar el pelo
	public void finCliente(){
		enter();
			// Sacamos al cliente de la silla (estaba en espera la hebra mientras se pelaba)
			 System.out.println("\033[31m"+"Barbero termina de afeitar " + "\033[0m");
			 sem_silla.signal();//Liberamos la silla
			
		leave();
	}

}


class Cliente implements Runnable{ 
	public Thread thr ;
	private Barberia barberia;
	
	public Cliente (Barberia brb){
		barberia=brb;
		thr=new Thread(this,"Cliente");
	}
		
	public void run (){
		while (true) {
			try{
				barberia.cortarPelo(); // el cliente espera (si procede) y se corta el pelo
				aux.dormir_max( 2000 ); // el cliente está fuera de la barberia un tiempo
			}catch(Exception e){
				System.err.println("Excepcion en main: " + e);
			}
		}
	}
}


class Barbero implements Runnable{ 
	
	public Thread thr ;
	private Barberia barberia;
	
	public Barbero(Barberia brb){
		barberia=brb;
		thr=new Thread(this,"Barbero");
	}
	
	
	public void run (){ 
		while (true) {
			try{
				barberia.siguienteCliente();
				aux.dormir_max( 2500 ); // el barbero está cortando el pelo
				barberia.finCliente();
			}catch(Exception e){
				System.err.println("Excepcion en main: " + e);
			}
		}
	}
}


class aux{
    static Random randomize = new Random();
    static void dormir_max(int milisecsMax){
		
        try{
            Thread.sleep(randomize.nextInt(milisecsMax));
        }catch(InterruptedException e){
            System.err.println("sleep interumpido en 'aux.simular_fumar()'");
        }
        
    }
}



class BarberoDurmiente{
	
	public static void main(String[] args){
        final int NUM_CLIENTES = 5;

        // leer parametros, crear vectores y buffer intermedio
        Barberia barberia = new Barberia();

        // crear hebras
        Barbero barbero = new Barbero(barberia);
        Cliente[] clientes = new Cliente[NUM_CLIENTES];
        for (int i=0; i<NUM_CLIENTES; i++)
            clientes[i] = new Cliente(barberia);


        // poner en marcha las hebras
      

        for (int i=0; i<NUM_CLIENTES; i++)
            clientes[i].thr.start();
            
              barbero.thr.start();

        aux.dormir_max( 8500 ); // el barbero esta cortando el pelo

        

    }
	
}
