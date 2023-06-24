//-------------- INCLUDES --------------//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "p3200278-p3200149-p3200227-res.h"

//--------- GLOBAL VARIABLES----------//
pthread_mutex_t lock;
pthread_cond_t cond;

pthread_mutex_t lock1;
pthread_cond_t cond1;

pthread_mutex_t lock2;
pthread_mutex_t lock3;

pthread_mutex_t lock4;

pthread_mutex_t lock5;

unsigned int seed;
int **pin;
int *costs;
char *zones;
int *first_seats;
int *tickets;
int available_phones = phones;
int available_cashiers = cashiers;
int money = 0;
int *temp_rows;
int random_sleep1;
int random_sleep2;
int seat_failure_counter = 0;
int card_failure_counter = 0;
int success_counter = 0;
double time_spent = 0.0;
double time_spent_waiting = 0.0;
double temp_time = 0.0;

//------ PROBABILITY GENERATOR FUNCTION ---------//

float prob_generator(int id)
{
	unsigned int temp_seed = time(NULL) * seed + id;
	float prob = (float)rand_r(&temp_seed) / RAND_MAX;

	return prob;
}

//------------ SEAT CHECKER -----------------//

int seat_checker(int first_row, int last_row, int tickets, int id)
{
	int counter;
	int current_seat;
	for (int current_row = first_row; current_row <= last_row; current_row++)
	{
		for (int first_seat = 0; first_seat <= 10 - tickets; first_seat++)
		{
			if (pin[current_row][first_seat] != 0)
			{
				continue;
			}
			else
			{
				counter = 1;
				current_seat = first_seat + 1;
				for (int ticks = 1; ticks <= tickets - 1; ticks++)
				{
					if (pin[current_row][current_seat] != 0)
					{
						break;
					}
					else
					{
						current_seat++;
						counter++;
					}
				}
				if (counter == tickets)
				{
					
					temp_rows[id] = current_row;

					return first_seat;
				}
			}
		}
	}
	printf("H krathsh apetyxe giati den yparxoyn katallhles theseis\n");
	seat_failure_counter++;
	return 20;
}

//----------- SEAT UPDATE FUNCTION --------------//

void seat_update(int id, int current_seat, int num_of_tickets, int mode, int temp_rows)
{
	
	for (int i = 1; i <= num_of_tickets; i++)
	{
		if (mode == 1)
		{
			pin[temp_rows][current_seat] = id;
		}
		else
		{
			pin[temp_rows][current_seat] = 0;
		}
		current_seat++;

		printf("Seat updated...\n");
	}
}

//----------- BOOKING FUNCTION --------------//

void *booking(void *x)
{

	struct timespec start, stop;
	struct timespec start2, stop2;
	struct timespec start3, stop3;

	int rc;
	int rc2;
	int rc3;
	int rc4;

	if (clock_gettime(CLOCK_REALTIME, &start) == -1)
	{							 // start time
		perror("clock gettime"); // error handling
		exit(EXIT_FAILURE);
	}

	int id = *(int *)x; // id of the current thread

	printf("The customer %d is on the line\n", id);
	rc = pthread_mutex_lock(&lock); // lock
	if (rc != 0)
	{
		printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
		pthread_exit(&rc);
	}

	if (clock_gettime(CLOCK_REALTIME, &start2) == -1)
	{							 // start time
		perror("clock gettime"); // error handling
		exit(EXIT_FAILURE);
	}

	while (available_phones == 0)
	{
		printf("The customer %d is waiting on the line...\n", id);
		rc = pthread_cond_wait(&cond, &lock);
		if (rc != 0)
		{
			printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
			pthread_exit(&rc);
		}
	}

	if (clock_gettime(CLOCK_REALTIME, &stop2) == -1)
	{							 // start time
		perror("clock gettime"); // error handling
		exit(EXIT_FAILURE);
	}

	time_spent_waiting += (stop2.tv_sec - start2.tv_sec);
	// costumer no longer waiting on the line
	printf("The customer %d is being serviced.\n", id);
	available_phones--;				  // using the resource
	rc = pthread_mutex_unlock(&lock); // unlock
	if (rc != 0)
	{
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
	}
	unsigned int temp_seed = time(NULL) * seed + id;
	random_sleep1 = (rand_r(&temp_seed) % (tHigh - tSlow + 1)) + tSlow; // genarate random number
	sleep(random_sleep1);												// sleep for that random random

	rc = pthread_mutex_lock(&lock); // lock
	if (rc != 0)
	{
		printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
		pthread_exit(&rc);
	}

	temp_seed = time(NULL) * seed + id;
	tickets[id] = rand_r(&temp_seed) % seatHigh + seatLow;
	float zone = prob_generator(id); // generate a propability
	int temp_rows2;

	if (zone <= 0.3)
	{

		rc3 = pthread_mutex_lock(&lock2); // lock
		if (rc3 != 0)
		{
			printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc3);
			pthread_exit(&rc3);
		}
		first_seats[id] = seat_checker(0, 9, tickets[id], id);
		rc3 = pthread_mutex_unlock(&lock2); // unlock
		if (rc3 != 0)
		{
			printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc3);
			pthread_exit(&rc3);
		}
		if (first_seats[id] != 20)
		{
			rc3 = pthread_mutex_lock(&lock2); // lock
			if (rc3 != 0)
			{
				printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc3);
				pthread_exit(&rc3);
			}
			seat_update(id, first_seats[id], tickets[id], 1, temp_rows[id]); // book seats
			rc3 = pthread_mutex_unlock(&lock2);								 // unlock
			if (rc3 != 0)
			{
				printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc3);
				pthread_exit(&rc3);
			}
			pthread_mutex_lock(&lock5);
			costs[id] = costA * tickets[id]; // increase cost
			pthread_mutex_unlock(&lock5);
			zones[id] = 'A';
		}
	}
	else
	{
		rc3 = pthread_mutex_lock(&lock2);
		if (rc3 != 0)
		{
			printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc3);
			pthread_exit(&rc3);
		}
		first_seats[id] = seat_checker(10, 29, tickets[id], id);
		rc3 = pthread_mutex_unlock(&lock2);
		if (rc3 != 0)
		{
			printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc3);
			pthread_exit(&rc3);
		}
		if (first_seats[id] != 20)
		{
			rc3 = pthread_mutex_lock(&lock2);
			if (rc3 != 0)
			{
				printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc3);
				pthread_exit(&rc3);
			}
			seat_update(id, first_seats[id], tickets[id], 1, temp_rows[id]); // book seats
			rc3 = pthread_mutex_unlock(&lock2);
			if (rc3 != 0)
			{
				printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc3);
				pthread_exit(&rc3);
			}
			pthread_mutex_lock(&lock5);
			costs[id] = costB * tickets[id]; // increase cost
			zones[id] = 'B';
			pthread_mutex_unlock(&lock5);
		}
	}
	available_phones++; // add resource back
	if (zone > 0.3)
	{
		temp_rows2 = temp_rows[id] - 10;
	}
	else
	{
		temp_rows2 = temp_rows[id];
	}
	rc = pthread_cond_signal(&cond); // signal condition
	if (rc != 0)
	{
		printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
		pthread_exit(&rc);
	}
	rc = pthread_mutex_unlock(&lock); // unlock
	if (rc != 0)
	{
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
	}

	rc2 = pthread_mutex_lock(&lock1); // lock
	if (rc2 != 0)
	{
		printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc2);
		pthread_exit(&rc2);
	}

	if (clock_gettime(CLOCK_REALTIME, &start3) == -1)
	{							 // start time
		perror("clock gettime"); // error handling
		exit(EXIT_FAILURE);
	}

	while (available_cashiers == 0)
	{
		printf("The customer %d is waiting to pay...\n", id);
		rc2 = pthread_cond_wait(&cond1, &lock1);
		if (rc2 != 0)
		{
			printf("ERROR: return code from pthread_cond_wait() is %d\n", rc2);
			pthread_exit(&rc2);
		}
	}

	if (clock_gettime(CLOCK_REALTIME, &stop3) == -1)
	{							 // start time
		perror("clock gettime"); // error handling
		exit(EXIT_FAILURE);
	}

	time_spent_waiting += (stop3.tv_sec - start3.tv_sec);

	available_cashiers--; // using the resource

	rc2 = pthread_mutex_unlock(&lock1); // unlock
	if (rc2 != 0)
	{
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc2);
		pthread_exit(&rc2);
	}
	temp_seed = time(NULL) * seed + id;
	random_sleep2 = (rand_r(&temp_seed) % (cashHigh - cashLow + 1)) + cashLow; // genarate random numbers

	sleep(random_sleep2);

	rc2 = pthread_mutex_lock(&lock1); // lock
	if (rc2 != 0)
	{
		printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc2);
		pthread_exit(&rc2);
	}

	if (first_seats[id] != 20)
	{
		printf("The customer %d is paying.\n", id);
		rc4 = pthread_mutex_lock(&lock3);
		if (rc4 != 0)
		{
			printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc4);
			pthread_exit(&rc4);
		}
		if (prob_generator(id) <= 0.9)
		{

			money += costs[id]; // add money to account
			rc4 = pthread_mutex_unlock(&lock3);
			if (rc4 != 0)
			{
				printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc4);
				pthread_exit(&rc4);
			}
			printf("H krathsh oloklhrwthke epityxws. Oi theseis sas einai sth zwnh %c/Seira %d/Arithmos ", zones[id], temp_rows2 + 1);
			success_counter++;
			for (int i = 0; i < tickets[id]; i++)
			{
				printf("%d ", first_seats[id] + i + 1);
			}
			printf("kai to kostos synallaghs %d eyrw\n", costs[id]);
		}
		else
		{
			seat_update(id, first_seats[id], tickets[id], 0, temp_rows[id]);
			rc4 = pthread_mutex_unlock(&lock3);
			if (rc4 != 0)
			{
				printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc4);
				pthread_exit(&rc4);
			}
			printf("H krathsh apetyxe giati h synallagh me thn pistwtikh karta den egine apodekti\n");
			card_failure_counter++;
		}
	}

	available_cashiers++;

	rc2 = pthread_cond_signal(&cond1); // signal condition
	if (rc2 != 0)
	{
		printf("ERROR: return code from pthread_cond_signal() is %d\n", rc2);
		pthread_exit(&rc2);
	}
	rc2 = pthread_mutex_unlock(&lock1); // pthread_mutex_unlock
	if (rc2 != 0)
	{
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc2);
		pthread_exit(&rc2);
	}

	if (clock_gettime(CLOCK_REALTIME, &stop) == -1)
	{							 // timer stop
		perror("clock gettime"); // error handling
		exit(EXIT_FAILURE);
	}
	temp_time = (stop.tv_sec - start.tv_sec); // time converting
	time_spent += (temp_time);

	pthread_exit(NULL); // quit thread
}

//----------- MAIN FUNCTION ---------------//

int main(int argc, char *argv[])
{

	printf("The program just started...\n");
	int rc2;
	int rc3;
	int rc4;

	//--------------- ERROR HANDLING--------------//
	if (argc != 3)
	{

		printf("Wrong amount of arguments. Exit...\n");

		return -1;
	}

	//---------- TYPE CASTING ---------------//
	int number_of_threads = atoi(argv[1]);
	seed = atoi(argv[2]);
	//---------- CHECK ARGUMENTS -----------//
	printf("thread amount: %d", number_of_threads);
	printf(" seed number: %d\n", seed);

	//---------- DYNAMIC ALLOCATION OF 2D ARRAY ---------//
	pin = malloc(sizeof(int *) * (front_rows + back_rows));

	for (int i = 0; i < front_rows + back_rows; i++)
	{
		pin[i] = malloc(sizeof(int) * seats_per_row);
	}

	costs = malloc(sizeof(int *) * (number_of_threads));
	zones = malloc(sizeof(int *) * (number_of_threads));
	tickets = malloc(sizeof(int *) * (number_of_threads));
	first_seats = malloc(sizeof(int *) * (number_of_threads));
	temp_rows = malloc(sizeof(int *) * (number_of_threads));

	//---------- ID INITIALIZING -----------//

	int *id = (int *)malloc(number_of_threads * sizeof(int));
	pthread_t threads[number_of_threads]; // thread array
	int rc;

	rc3 = pthread_mutex_init(&lock2, NULL); // mutex initialization
	if (rc3 != 0)
	{
		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc3);
		exit(-1);
	}
	rc4 = pthread_mutex_init(&lock3, NULL); // mutex initialization
	if (rc4 != 0)
	{
		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc4);
		exit(-1);
	}

	rc2 = pthread_mutex_init(&lock1, NULL); // mutex initialization
	if (rc2 != 0)
	{
		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc2);
		exit(-1);
	}
	rc2 = pthread_cond_init(&cond1, NULL); // condition initialization
	if (rc2 != 0)
	{
		printf("ERROR: return code from pthread_cond_init() is %d\n", rc2);
		exit(-1);
	}

	rc = pthread_mutex_init(&lock, NULL); // mutex initialization
	if (rc != 0)
	{
		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
		exit(-1);
	}
	rc = pthread_cond_init(&cond, NULL); // condition initialization
	if (rc != 0)
	{
		printf("ERROR: return code from pthread_cond_init() is %d\n", rc);
		exit(-1);
	}
	
	for (int rows = 0; rows <= front_rows + back_rows - 1; rows++)
	{
		for (int seat_num = 0; seat_num <= seats_per_row - 1; seat_num++)
		{
			pin[rows][seat_num] = 0;
		}
	}
	//-------------- CREATING THREAD AND ASSIGNING IT -------------//
	for (int i = 0; i < number_of_threads; i++)
	{
		id[i] = i + 1;
		if (i >= 1)
			printf("Creating Thread %d...\n", id[i]);
		rc = pthread_create(&threads[i], NULL, booking, &id[i]);
		if (rc != 0)
		{
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
		unsigned int temp_seed = time(NULL) * seed + id[i];
		int r = (rand_r(&temp_seed) % (resHigh - resLow + 1)) + resLow;
		sleep(r);
	}
	//--------------- JOINING THREADS -----------------//
	for (int i = 0; i < number_of_threads; i++)
	{
		rc = pthread_join(threads[i], NULL);
		if (rc != 0)
		{
			printf("ERROR: return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
	}
	//-------------- PRINTING THE PIN ARRAY -------------//
	
	printf("===========================================================================================================================\n");
	int temp_row;
	char zone;
	for (int r = 0; r <= front_rows + back_rows - 1; r++)
	{
		for (int s = 0; s <= seats_per_row - 1; s++)
		{
			if (pin[r][s] != 0)
			{
				if (r > 9)
				{
					temp_row = r - 10;
					zone = 'B';
				}
				else
				{
					temp_row = r;
					zone = 'A';
				}
				printf("Zwnh %c / Seira %d / Thesi %d", zone, temp_row + 1, s + 1);
				int temp = pin[r][s];
				for (int i = s; i <= seats_per_row - 2; i++)
				{
					if (pin[r][i + 1] == temp)
					{
						printf(" %d", i + 2);
					}
					else if (pin[r][i + 1] != 0)
					{
						printf(" / Pelatis %d", pin[r][i]);
						temp = pin[r][i + 1];
						printf(" , Thesi %d", i + 2);
					}
				}
				printf(" / Pelatis %d", temp);
				printf("\n");
				break;
			}

			continue;
		}
	}
	printf("===========================================================================================================================");
	printf("\nTa synolika esoda apo tis pwlhseis einai: %d\n", money);
	double success_rate_ = (double)success_counter / (double)number_of_threads * 100;
	double seat_failure_rate = (double)seat_failure_counter / (double)number_of_threads * 100;
	double card_failure_rate = (double)card_failure_counter / (double)number_of_threads * 100;

	printf("Pososto Epityxias: %.2f %%\nPososto Apotyxias logo mh evreshs thesewn: %.2f %%\nPososto Apotyxias kartas: %.2f %%\n", success_rate_, seat_failure_rate, card_failure_rate);
	printf("==================================================================\n");

	time_spent = time_spent / number_of_threads;
	time_spent_waiting = time_spent_waiting / number_of_threads;

	printf("Average service time :  %.2f secs\n", time_spent);

	printf("Average waiting time :  %.2f secs\n", time_spent_waiting);
	printf("==================================================================\n");

	rc = pthread_mutex_destroy(&lock); // deleting mutex
	if (rc != 0)
	{
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
		exit(-1);
	}
	rc = pthread_cond_destroy(&cond); // deleting condition
	if (rc != 0)
	{
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1);
	}

	rc2 = pthread_mutex_destroy(&lock1); // deleting mutex
	if (rc2 != 0)
	{
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc2);
		exit(-1);
	}
	rc2 = pthread_cond_destroy(&cond1); // deleting condition
	if (rc2 != 0)
	{
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc2);
		exit(-1);
	}

	rc3 = pthread_mutex_destroy(&lock2); // deleting mutex
	if (rc3 != 0)
	{
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc3);
		exit(-1);
	}
	rc4 = pthread_mutex_destroy(&lock3); // deleting mutex
	if (rc4 != 0)
	{
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc4);
		exit(-1);
	}

	free(pin); // deleting array from memory
	free(id);
	free(tickets);
	free(first_seats);
	free(temp_rows);
	
	printf("The program is finished...\n");

	return 1;
}
