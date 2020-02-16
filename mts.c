#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<pthread.h> 
#include <sys/time.h>

typedef struct train{
	int id;
	char* prior;
	char* dir;
	float load_time; 	//e.g., service time from tut5
	float cross_time;
}train;

#define MAX 100
#define sleepTime 100000
#define BILLION 1000000000.0;
train trains[MAX];
train* queue[MAX];
int len = 0;
pthread_t threads[MAX];
pthread_mutex_t mutex;
pthread_cond_t convar;
struct timespec start;

void arrangeIntoStruct(char* dir, int load_time, int cross_time, int train_num)
{
	char* prior;
	char* side;

		if(strncmp(dir,"e", 1)==0)
    	{
    		prior = "low";
    		side = "East";
    	}else if(strncmp(dir,"w", 1)==0)
    	{
    		prior = "low";
    		side = "West";
    	}else if(strncmp(dir,"E", 1)==0)
    	{
    		prior = "high";
    		side = "East";
    	}else if(strncmp(dir,"W", 1)==0)
    	{
    		prior = "high";
    		side = "West";
    	}
    	 	
    	train t = { 
    		train_num++,
			prior,
    		side, 		  	//could be e or w || E or W
    		load_time*0.1, 
    		cross_time*0.1
    	};
    	//printf("%d\n",train_num);
		//printf("%0.1fs\n", t.load_time);
		//printf("%s\n", t.prior);
		//printf("%s\n", t.dir);
    	
    	trains[train_num-1] = t; 	
}

float TimeEval()
{
	struct timespec stop;
	double accum; 
	if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) 
	{
 		perror( "clock gettime" );
 		exit( EXIT_FAILURE );
 	}
 	accum = ( stop.tv_sec - start.tv_sec ) + ( stop.tv_nsec - start.tv_nsec )/ BILLION;
 	return ( "%lf\n", accum );
 	//return (double)(“%02d:%02d:%04.1f”, hours, minutes, seconds);

}

void makeQueue(train* t)
{
	queue[len] = t;
	len++;
}

//removes last inserted trian from the queue
void changeQueue()
{
	int x = 0;
	while(x < len-1)
	{
		queue[x] = queue[x+1];
		x+=1;
	}
	len-=1;
}

void* execCond(train* t)
{
	pthread_mutex_lock(&mutex);
	makeQueue(t);
	//order();
	if (t->id != queue[0]->id) {
		usleep(t->cross_time*sleepTime);
	}
	while(queue[0]->id != t->id)
	{
		if(pthread_cond_wait(&convar, &mutex)!=0)
		{
			printf("Can't wait for convar\n");
			exit(1);
		}
	}	
	pthread_mutex_unlock(&mutex);
}

void releaseCond(train* t)
{
	pthread_mutex_lock(&mutex);	
	pthread_cond_broadcast(&convar);
	changeQueue();
	pthread_mutex_unlock(&mutex);	
}

void* print_function(void* trainFlow)
{
	train* t = (train*)trainFlow;

	//wait for a train to be loaded
	usleep(t->load_time*sleepTime);

	printf("%.2f\tTrain %d is ready to go to %s\n", TimeEval(), t->id, t->dir);

	//printf("%0.1fs\n", t->cross_time);
	//printf("%s\n", t->prior);
	//printf("%s\n", t->dir);
	
	execCond(t);
	
	printf("%.2f\tTrain %d is ON the main track going %s\n",TimeEval(),t->id,t->dir);
	usleep(t->cross_time*sleepTime);
	
	printf("%.2f\tTrain %d is OFF the main track after going %s\n",TimeEval(),t->id,t->dir);

	releaseCond(t);

	pthread_exit(NULL);
} 

int main(int argc, char const *argv[])
{
	FILE* file = fopen(argv[1], "r");
	char dir[MAX];	//direction
	int load_time; 	//e.g., service time
	int cross_time;
	
    int train_num = -1, count = 0;   

	if(file == NULL)
	{
		printf("No trains to cross the track\n");
		exit(0);
	}

	while (EOF != fscanf(file, "%s %d %d\n", dir, &load_time, &cross_time))
    {	
    	train_num++;
    	arrangeIntoStruct(dir, load_time, cross_time, train_num);  	
    	count++;
    	
    }
    //printf("%d\n", count );
   
  	fclose(file);

  	// Initialization of mutex, convar, and attr; 
  	// as from tutorial slides
  	pthread_mutex_init(&mutex, NULL);
  	pthread_cond_init(&convar, NULL);
  	
  	pthread_attr_t attr; 
  	pthread_attr_init(&attr);
  	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  	

	if(clock_gettime( CLOCK_REALTIME, &start) == -1 ) 
	{
 		perror("clock gettime");
 		exit(EXIT_FAILURE);
 	}

	//thread for each flow
	int i;
	for(i = 0; i < count; i++)
	{
		pthread_create(&threads[i], NULL, print_function, (void*)&trains[i]);
	}

	//wait 
	for(i = 0; i < count; i++)
	{
		pthread_join(threads[i], NULL);
	}

	//Destroy all
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&convar);
	pthread_attr_destroy(&attr);
	
	pthread_exit(NULL);

  	return 0;
	
}












