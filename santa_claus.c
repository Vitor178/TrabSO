#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define REINDEER 9 
#define ELFS 100

int count_elf = 0; 
int count_reindeer = 0; 	 
sem_t sem_santa, sem_elf, sem_reindeer;

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elf_mutex = PTHREAD_MUTEX_INITIALIZER;

void get_hitched (){
	printf("\nGot hitch");
	fflush(stdout);
}
void get_help (){
	printf("\nGot help");
	fflush(stdout);
}
void get_Sleight(){
	printf("\nGot Sleight");
	fflush(stdout);
}
void help_elf(){
	printf("\nElf OK");
	fflush(stdout);
}

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


void *Santa(){
	while(1){
	sem_wait(&sem_santa);
	if (count_reindeer == REINDEER){
		for(int p=0;p<9;p++){
		pthread_mutex_lock(&count_mutex);
		count_reindeer--;
		pthread_mutex_unlock(&count_mutex);
			sem_post(&sem_reindeer);
			get_Sleight();
		}

	}
	else{
	  sem_post(&sem_elf);
	  help_elf();
	  sem_post(&sem_elf);
	  help_elf();
	  sem_post(&sem_elf);
	  help_elf();

	}	
	}
}


int main (){
	int ticket=0;
	sem_init(&sem_elf,0,0);
	sem_init(&sem_santa,0,0);
	sem_init(&sem_reindeer,0,0);
	pthread_t Reindeerthreads[REINDEER];
	pthread_t Elfthreads[ELFS];
	pthread_t Santathreads[1];
	srand(time(NULL));
	int cont=0;
	pthread_create(&Santathreads[0],NULL,Santa,NULL);
	while(1){
		sleep(1);
		printf("\n%i",cont);
		ticket = rand()%4;
		switch(ticket){
			case 0:
				pthread_mutex_lock(&count_mutex);
				if(count_reindeer < REINDEER){
				printf("\nCriou Reindeer");
				pthread_create(&Reindeerthreads[cont], NULL, reindeer, NULL);
				};
				pthread_mutex_unlock(&count_mutex);
				break;
			default:
			        if(cont <ELFS)	
					printf("\nCriou Elf");
					pthread_create(&Elfthreads[cont], NULL, elf, NULL);
				break;
		}

		cont ++;

	}	




}





