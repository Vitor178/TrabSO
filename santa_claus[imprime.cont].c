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

	printf("Papai noel: dormindo \nElfos: %d  Renas : %d\n",count_elf, count_reindeer);
	fflush(stdout);
}

//Thread executada pelas renas
void *reindeer(){
	pthread_mutex_lock(&count_mutex);	//impede que mais alguem altere o valor
	count_reindeer++;
	printf("\n-------------------------\nCriou Reindeer   ");	//usado apenas para visualizacao
	imprime();							//usado apenas para visualizacao
	if (count_reindeer==REINDEER)				//se ja tiverem 9 acorda o santa, ainda impedindo que o valor mude antes de verificar
		sem_post(&sem_santa);
	pthread_mutex_unlock(&count_mutex);			//permite que outros reindeers alterem o valor
	sem_wait(&sem_reindeer);			//espera o santa liberar ele
	
	get_hitched();					
	pthread_exit(NULL);
	
}

//Thread executada pelos elfos
void *elf(){
	pthread_mutex_lock(&elf_mutex);  				//impede que outros elfos iniciem
	pthread_mutex_lock(&count_mutex); 				//impede que mudem o valor do contador
       	count_elf ++;
	printf("\n-------------------------\nCriou Elf        ");	//usado apenas para visualizacao
	imprime();							//usado apenas para visualizacao
	if (count_elf == 3)						//verifca se tem 3 elfos esperando
		sem_post(&sem_santa);					//acorda o santa
	else 
		pthread_mutex_unlock(&elf_mutex);  			//se nao tiverem 3 esperando, permite que outros cheguem
	pthread_mutex_unlock(&count_mutex);  				//permite que alterem os conts 
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
	printf("Papai Noel acordou!!!\n");
	fflush(stdout);
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
	sem_init(&sem_elf,0,0);         //inicia os semafaros
	sem_init(&sem_santa,0,0);
	sem_init(&sem_reindeer,0,0);
	pthread_t Reindeerthreads[REINDEER];
	pthread_t Elfthreads[ELFS];
	pthread_t Santathreads[1];
	srand(time(NULL)); 			//necessario para aleatorizar a criacao
	int contE=0, contR=0; 			//Contador contE para limitar o numero de elfos. Ambos sao usados na pthread_create
	if (pthread_create(&Santathreads[0],NULL,Santa,NULL)){
		printf ("Nao foi possivel criar o Santa");
		exit(EXIT_FAILURE);
	}
	while(1){
		sleep(1);					//Para facilitar a visualizacao
		//printf("\n%i",cont);
		ticket = rand()%4;     //Sorteia o ticket
		switch(ticket){					//Cria as treads aleatoriamente com uma chance em quatro de criar reindeer
			case 0:
				pthread_mutex_lock(&count_mutex);	//bloqueia o mutex para impedir que o valor mude após a verificação
				if(count_reindeer < REINDEER){		//impedindo assim que se tenha mais de 9 reindeers
				if (pthread_create(&Reindeerthreads[contR], NULL, reindeer, NULL)){	//Criação da thread Reindeer
					printf ("Nao foi possivel criar o Reindeer");
					exit(EXIT_FAILURE);
				}
				};
				pthread_mutex_unlock(&count_mutex);
				contR ++;				//aumenta para o vetor Reindeerthreads
				break;
			default:				//Nao e necessario bloquear o cont_mutex aqui. Essas variaveis sao apenas da main		
			        if(contE <ELFS)	
					if(pthread_create(&Elfthreads[contE], NULL, elf, NULL)){	// Criação da thread Elf
						printf ("Nao foi possivel criar o Reindeer");
						exit(EXIT_FAILURE);
					}
				contE ++;			//aumenta para o vetor Elfthreads
				break;
		}

	}	




}





