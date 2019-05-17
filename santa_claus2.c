#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>


//Define o valores maximos de renas e elfos
#define REINDEER 9 
#define ELFS 100000

//Inicialização dos contadores
int count_elf = 0; 
int count_reindeer = 0; 	 

//Inicialização dos semaforos
sem_t sem_santa, sem_elf, sem_elf_num, sem_reindeer;

//Inicialização dos mutex
pthread_mutex_t elf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;


// Funções executadas pelos atores do problema
void get_hitched (){
	printf("\nRena pronta\n");
	fflush(stdout);
}
void get_help (){
	printf("\nElfo recebeu ajuda\n");
	fflush(stdout);
}
void get_Sleight(){
	printf("\nPreeparar rena");
	fflush(stdout);
}
void help_elf(){
	printf("\nElfo ajudado");
	fflush(stdout);
}

//Função que imprime a quantidade de renas e elfos ja criados
void imprime(){

	printf("Papai noel: dormindo \nElfos: %d  Renas : %d\n",count_elf, count_reindeer);
	fflush(stdout);
}

//Thread executada pelas renas
void *reindeer(){
	while(1) {
		//sleep((rand()%700)/1000);
		pthread_mutex_lock(&reindeer_mutex);				//impede que outra thread altere o valor
		count_reindeer++;
		printf("\n-------------------------\nCriou Reindeer   ");	//usado apenas para visualizacao
		imprime();							//usado apenas para visualizacao
		if (count_reindeer==REINDEER)		//se ja tiverem 9 acorda o santa, ainda impedindo que o valor mude antes de verificar
			sem_post(&sem_santa);
		pthread_mutex_unlock(&reindeer_mutex);				//permite que outros threads alterem o valor
		sem_wait(&sem_reindeer);					//espera o santa liberar ele
		
		get_hitched();		
	}
	
}

//Thread executada pelos elfos
void *elf(){
	sem_wait(&sem_elf_num); 				
	pthread_mutex_lock(&elf_mutex); 				//impede que mudem o valor do contador
       	count_elf ++;
	printf("\n-------------------------\nCriou Elf        ");	//usado apenas para visualizacao
	imprime();							//usado apenas para visualizacao
	if (count_elf == 3)						//verifca se tem 3 elfos esperando
		sem_post(&sem_santa);					//acorda o santa
	pthread_mutex_unlock(&elf_mutex);  				//permite que alterem os conts 
	sem_wait(&sem_elf);						//espera o santa liberar ele
	get_help();
	pthread_mutex_lock(&elf_mutex);  
	count_elf--;
	if(count_elf == 0)					//se for o ultimo a reduzir libera para que outros esfos possam entrar
		for(int a=0; a<3; a++){
			sem_post(&sem_elf_num);
		} 
	pthread_mutex_unlock(&elf_mutex);  				//permite que outros alterem o cont
	

	pthread_exit(NULL);

}

//Thread executada pelo Papai Noel
void *Santa(){

	while(1){				//santa fica em um loop
		sem_wait(&sem_santa);			//dorme ate ser chamado pelo reeindeer ou pelo elf
		printf("\n-------------------------\n");//usado apenas para visualizacao
		printf("Papai Noel acordou!!!\n");	//usado apenas para visualizacao
		fflush(stdout);				//usado apenas para visualizacao
		pthread_mutex_lock(&reindeer_mutex);	
		if (count_reindeer == REINDEER){
			for(int p=0; p<9; p++){		// Se existem 9 reindeers então elas sao atendidas primeiramente
				count_reindeer--;
				get_Sleight();
				sem_post(&sem_reindeer);
			}
			pthread_mutex_unlock(&reindeer_mutex);
		}
		else{					//caso nao haja 9 reindeers, ele foi acordado pelos elfs
			pthread_mutex_unlock(&reindeer_mutex);
			pthread_mutex_lock(&elf_mutex);
			for(int e=0;e<3;e++){	
				help_elf();
	 			sem_post(&sem_elf);    		//Libera elfos e os ajuda
			}
			pthread_mutex_unlock(&elf_mutex);
		}
	}
}

void *Create_elf(){
	pthread_t Elfthreads[ELFS];
	int cont=0;
	while(1){
		//sleep((rand()%300)/1000);
		if(pthread_create(&Elfthreads[cont], NULL, elf, NULL)){	// Criação da thread Elf
			printf ("Nao foi possivel criar o Reindeer");
			fflush(stdout);
			exit(EXIT_FAILURE);
		}
		//cont++;
	}
	//pthread_exit(NULL);
}
int main (){
	srand(time(NULL));
	sem_init(&sem_elf,0,0);         //inicia os semafaros
	sem_init(&sem_santa,0,0);
	sem_init(&sem_reindeer,0,0);
	sem_init(&sem_elf_num,0,3);
	pthread_t Reindeerthreads[REINDEER];
	pthread_t Santathreads[1];
	pthread_t Elfcreat[1];
	srand(time(NULL)); 			//necessario para aleatorizar a criacao
	if (pthread_create(&Santathreads[0],NULL,Santa,NULL)){
		printf ("Nao foi possivel criar o Santa");
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
	for (int i=0; i<9; i++){
		if (pthread_create(&Reindeerthreads[i], NULL, reindeer, NULL)){	//Criação da thread Reindeer
			printf ("Nao foi possivel criar o Reindeer");
			fflush(stdout);	
			exit(EXIT_FAILURE);
		}		
	}
	if (pthread_create(&Elfcreat[0],NULL,Create_elf,NULL)){
		printf ("Nao foi possivel criar o Santa");
		fflush(stdout);	
		exit(EXIT_FAILURE);
	}
	pthread_exit(NULL);
}






