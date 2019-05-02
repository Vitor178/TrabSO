#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define REINDEER 9 

int count_elf = 0; 
int count_reindeer = 0; 	 
sem_t sem_santa, sem_elf, sem_reindeer;

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elf_mutex = PTHREAD_MUTEX_INITIALIZER;

void get_hitched (){
	printf("Got hitch");
}
void get_help (){
	printf("Got help");

void get_Sleight(){

}
}
void *reindeers(void *n_tava_indo_sem_isso){
	pthread_mutex_lock(&count_mutex);	//impede que mais alguem altere o valor
	count_reindeer++;
	if (count_reindeer==REINDEER)				//se ja tiverem 9
		sem_post(&sem_santa);
	pthread_mutex_unlock(&count_mutex);
	sem_wait(&sem_reindeer);			//espera o santa liberar ele
	
	get_hitched();
	
}

void *elfs(){
	pthread_mutex_lock(&elf_mutex);  
	pthread_mutex_lock(&count_mutex);  
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
}

void *Santa(void *num){
	int *aux = (int *)num;
	long int id = (int)*aux;
	while(1){
	sem_wait(&sem_santa);
	if (count_reindeer == REINDEER){
		pthread_mutex_lock(&count_mutex);
		count_reindeer--;
		pthread_mutex_unlock(&count_mutex);
		sem_post(&sem_reindeer);
		getSleight();
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
	pthread_exit(NULL);	
}


int main (){
	pthread_t threads[REINDEER];
	long int cont;
	
	for (cont = 0; cont<REINDEER; cont++){
		for(int time=0;time<1000;time++);
		printf("\nCreate: %li",cont);
		pthread_create(&threads[cont], NULL, reindeers, NULL);
	}



}





