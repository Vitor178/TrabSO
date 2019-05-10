#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>


//Define o valores maximos de renas e elfos
#define REINDEER 9 
#define ELFS 100

//Inicialização dos contadores
int count_elf = 0; 
int count_reindeer = 0; 	 

//Inicialização dos semaforos
sem_t sem_santa, sem_elf, sem_reindeer;

//Inicialização dos mutex
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elf_mutex = PTHREAD_MUTEX_INITIALIZER;


// Funções executadas pelos atores do problema
void get_hitched (){
	printf("Got hitch\n");
	fflush(stdout);
}
void get_help (){
	printf("Got help\n");
	fflush(stdout);
}
void get_Sleight(){
	printf("Got Sleight\n");
	fflush(stdout);
}
void help_elf(){
	printf("Elf OK\n");
	fflush(stdout);
}

//Função que imprime a quantidade de renas e elfos ja criados
void imprime(){

	printf(" Elfos: %d  Renas : %d\n",count_elf+1, count_reindeer+1);
	fflush(stdout);
}

//Thread executada pelas renas
void *reindeer(void *n_tava_indo_sem_isso){
	pthread_mutex_lock(&count_mutex);	//impede que mais alguem altere o valor
	count_reindeer++;
	if (count_reindeer==REINDEER)				//se ja tiverem 9
		sem_post(&sem_santa);
	pthread_mutex_unlock(&count_mutex);
	sem_wait(&sem_reindeer);			//espera o santa liberar ele
	
	get_hitched();
	pthread_exit(NULL);
	
}

//Thread executada pelos elfos
void *elf(){
	pthread_mutex_lock(&elf_mutex);  
	pthread_mutex_lock(&count_mutex); 
       	count_elf ++;	
	if (count_elf == 3)
		sem_post(&sem_santa);
	else 
		pthread_mutex_unlock(&elf_mutex);  
	pthread_mutex_unlock(&count_mutex);  
	sem_wait(&sem_elf);
	get_help();
	pthread_mutex_lock(&count_mutex);  
	count_elf--;
	if(!count_elf)
		pthread_mutex_unlock(&elf_mutex); 
	pthread_mutex_unlock(&count_mutex);  
	

	pthread_exit(NULL);

}

//Thread executada pelo Papai Noel
void *Santa(){
	while(1){
	sem_wait(&sem_santa);
	pthread_mutex_lock(&count_mutex);
	if (count_reindeer == REINDEER){
		for(int p=0;p<9;p++){		// Se existem 9 renas então elas sao atendidas primeiramente
		count_reindeer--;
			sem_post(&sem_reindeer);
			get_Sleight();
		}
		

	}
	else{
	for(int e=0;e<3;e++){	
	  sem_post(&sem_elf);    //Libera elfos e os ajuda
	  help_elf();  

	}
	}
	pthread_mutex_unlock(&count_mutex);	
	}
}


int main (){
	int ticket=0; 			//Inicialização de ticket, que sera usado para sortear qual thread sera criada
	sem_init(&sem_elf,0,0);         
	sem_init(&sem_santa,0,0);
	sem_init(&sem_reindeer,0,0);
	pthread_t Reindeerthreads[REINDEER];
	pthread_t Elfthreads[ELFS];
	pthread_t Santathreads[1];
	srand(time(NULL)); 
	int cont=0; 			//Contador para que o numero de elfos criados nao ultrapasse o limite
	pthread_create(&Santathreads[0],NULL,Santa,NULL);
	while(1){
		sleep(1);
		//printf("\n%i",cont);
		ticket = rand()%4;     //Sorteia o ticket
		switch(ticket){
			case 0:
				pthread_mutex_lock(&count_mutex);
				if(count_reindeer < REINDEER){
				pthread_create(&Reindeerthreads[cont], NULL, reindeer, NULL);	//Criação da thread Reindeer
				printf("Criou Reindeer   "); imprime();
				};
				pthread_mutex_unlock(&count_mutex);
				break;
			default:
				pthread_mutex_lock(&count_mutex);
			        if(cont <ELFS)	
					pthread_create(&Elfthreads[cont], NULL, elf, NULL);	// Criação da thread Elf
					printf("Criou Elf        "); imprime();
				pthread_mutex_unlock(&count_mutex);
				break;
		}

		cont ++;

	}	




}





