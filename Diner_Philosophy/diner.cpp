/*
	Owen, Zachary
	Vu, Don

	OS Concepts 351
	Diners Philosophy
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <cstdio>

#define MAX_SECONDS_OF_SLEEP 3
#define AMOUNT_OF_SECONDS rand() % MAX_SECONDS_OF_SLEEP + 1
#define CAPACITY_OF_MEALS_FOR_EACH 3

const int NUM_PHILOSOPHERS = 5;
int numberOfMeals[5] = { 0 };

enum condition {THINKING, HUNGRY, EATING};
condition state[5] = {THINKING};

pthread_mutex_t maitre_d;
pthread_cond_t cond_var, cond_var2, cond_var3, cond_var4, cond_var5;
pthread_cond_t c_v[5] = {cond_var, cond_var2, cond_var3, cond_var4, cond_var5};

void* dinner_table(void* philosopher_number);
void pickup_forks(int philosopher_number);
void return_forks(int philosopher_number);
void check_diner_table(int philosopher_number);
void print_status();
void initialize(); 
void destroy();	

int main(void) {
	srand(time(NULL));
	initialize(); // Initialize mutex and condition variables.

	printf("========================================================\n");
	printf("|               ===== PHILOSOPHERS =====               |\n");
	printf("========================================================\n");
	printf("| Phil_0   |  Phil_1  |  Phil_2  |  Phil_3  |  Phil_4  |\n");

	print_status();	


	/* Begin by creating 5 philosophers, each identified by a number 0..4 */
	pthread_t philosopher[5];
	for (int id = 0; id < NUM_PHILOSOPHERS; id++) { 
		pthread_create(&philosopher[id], NULL, dinner_table, (void*)id);
	}
	
	for (int id = 0; id < NUM_PHILOSOPHERS; id++){
		pthread_join(philosopher[id], NULL);
	}
	    
	void destroy();	// Destory mutex and condition variables.
    	printf("\t\t...done.\n");
    	return 0;
}

void* dinner_table(void* philosopher_number) {
	int id = (int)philosopher_number;
	
	/* Each philosopher will have the same number of courses */
	while (numberOfMeals[id] < CAPACITY_OF_MEALS_FOR_EACH) {
		pickup_forks(id);
		return_forks(id);
	}
}

void pickup_forks(int philosopher_number) {
	sleep(AMOUNT_OF_SECONDS); // Thinking

	pthread_mutex_lock(&maitre_d); // Escort to table
	state[philosopher_number] = HUNGRY;

	check_diner_table(philosopher_number); // Maitre D checks to see if table is available.

	while(state[philosopher_number] == HUNGRY) // Seat unavailable (due to utensil shortage).
		pthread_cond_wait(&c_v[philosopher_number], &maitre_d);
	pthread_mutex_unlock(&maitre_d);
}

void return_forks(int philosopher_number) {
	sleep(AMOUNT_OF_SECONDS); // Eating

	pthread_mutex_lock(&maitre_d); // Clean table for next guest.
	state[philosopher_number] = THINKING;

	check_diner_table((philosopher_number + 4) % 5); // Seat to the left available.
	check_diner_table((philosopher_number + 1) % 5); // Seat to the right available.
	
	pthread_mutex_unlock(&maitre_d);
}

void check_diner_table(int philosopher_number) {
	int i = philosopher_number;
	/* If this philosopher is hungry and neighbors arn't eating. */
	if (state[i] == HUNGRY && (state[(i+4) % 5] != EATING) 
			       && (state[(i+1) % 5] != EATING)) {
		state[philosopher_number] = EATING; // Seat occupied.
		numberOfMeals[philosopher_number]++;
		print_status(); // Print right after incrementing meal.
		pthread_cond_signal(&c_v[philosopher_number]); // Utensil available
	}
}

void print_status() {
	printf("========================================================\n");
	for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
		if (state[i] == THINKING) {
			printf("| THINKING ");
		} else if (state[i] == HUNGRY) {
			printf("|  HUNGRY  ");
		} else {
			printf("| EATING(%d)", numberOfMeals[i]);
		}
	}
	printf("|");

	printf("\n");
}

void initialize(){
	pthread_mutex_init(&maitre_d, NULL); 
	pthread_cond_init(&cond_var, NULL);
	pthread_cond_init(&cond_var2, NULL);
	pthread_cond_init(&cond_var3, NULL);
	pthread_cond_init(&cond_var4, NULL);
}

void destroy() {
	pthread_mutex_destroy(&maitre_d);
	pthread_cond_destroy(&cond_var);
	pthread_cond_destroy(&cond_var2);
	pthread_cond_destroy(&cond_var3);
	pthread_cond_destroy(&cond_var4);
}

