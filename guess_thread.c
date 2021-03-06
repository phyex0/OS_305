/*

	Halit Burak Yeşildal		18050111043
	Ayşe Tüncer			17050111058
	Zeynep Nur Tüncel		17050111011
	Yakup Batuhan Ördek		18050111041
	Berna Altun			16050111040
	Enes Güler			18050111005

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

//global variable which is allowed to access by all threads.
char guess_by_threads[15][5]; //storing results
int global_index = 0;  // last assigned index
int done[] = { 0,0,0 }; //when a thread done with guess increases 1 for given index
int score[] = { 0, 0, 0 }; // 
int random_numbers[5];

//to prevent collision race condition mutex initialized.
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

//function declerations.
int random_number();
void guess(int array[]);
void* runner(void* param);
void compare(int round);
void substring(char* source, char* dest, int begin, int len);
void resulting();

int main() {
	srand(time(NULL));

	//computer made its own decision.
	guess(random_numbers);

	//run game...
	printf("The game has launched\n");
	printf("Computer choses [");
	for (int i = 0; i < 5; i++)
		printf("%d, ", random_numbers[i]);

	printf("] randomly\n");
	printf("3 threads will be created\n");
	printf("The game starts\n--\n");


	//defining threads' id
	pthread_t tid[4];

	//creating threads.
	for (int i = 0; i < 4; i++) {
		int* param = malloc(sizeof(*param));
		*param = i + 1;
		pthread_create(&tid[i], NULL, runner, param);

	}

	//terminating the threads.
	for (int i = 0; i < 4; i++)
		pthread_join(tid[i], NULL);

	resulting();

	//prints the guess_by_threads array
	printf("Guesses made by threads: [");
	for (int i = 0; i < 15; i++) {
		printf("%s, ", guess_by_threads[i]);
	}
	printf("] \n--\n--\n");

	for (int i = 1; i <= 3; i++)
		printf("%d. Thread terminated\n", i);

	printf("Threads are joined by main process \nGame Finished\n");


	return 0;
}

//runner method of theads.
void* runner(void* param)
{
	//type casting for the which thread.
	int tid = *(int*)param;

	for (int i = 0; i < 5; i++) {
		//First three thread makes guesses.
		if (global_index < 15) {
			if (tid != 4) {
				//for the race condition.
				pthread_mutex_lock(&lock);
				sprintf(guess_by_threads[global_index++], "%d_%d\0", tid, random_number());
				done[tid - 1] = 1;
				pthread_mutex_unlock(&lock);

				while (done[tid - 1] != 0);
			}

			//4th thred is controllik the rounds.
			else {

				while (!(done[0] == 1 && done[1] == 1 && done[2] == 1));

				if (done[0] == 1 && done[1] == 1 && done[2] == 1) {
					//make compare;
					compare(i);

					//make ready for the next round
					pthread_mutex_lock(&lock);
					for (int j = 0; j < 3; j++)
						done[j] = 0;
					pthread_mutex_unlock(&lock);

				}
			}


		}

	}

	pthread_exit(0);
}

int random_number()
{
	return rand() % 99 + 1;
}

void guess(int array[5])
{
	for (int i = 0; i < 5; i++)
		array[i] = random_number();
}

//Makes the compare for the guesses of theads.
void compare(int round) {
	int guesses[3];
	char number[3];
	for (int i = global_index - 3; i < global_index; i++) {
		substring(&guess_by_threads[i], &number, 2, strlen(guess_by_threads[i]));
		guesses[atoi(&guess_by_threads[i][0]) - 1] = atoi(number);
	}

	printf("Turn %d, Guesses: 1.thread : %d, 2.theard: %d, 3.thread: %d \n", round + 1, guesses[0], guesses[1], guesses[2]);

	if (abs(random_numbers[round] - guesses[0]) < abs(random_numbers[round] - guesses[1]) && abs(random_numbers[round] - guesses[0]) < abs(random_numbers[round] - guesses[2]))
	{
		score[0]++;
		printf("1. thread win, Score: %d - %d - %d, %d is closest to %d \n--\n--\n", score[0], score[1], score[2], guesses[0], random_numbers[round]);
	}

	else if (abs(random_numbers[round] - guesses[1]) < abs(random_numbers[round] - guesses[0]) && abs(random_numbers[round] - guesses[1]) < abs(random_numbers[round] - guesses[2]))
	{
		score[1]++;
		printf("2. thread win, Score: %d - %d - %d, %d is closest to %d \n--\n--\n", score[0], score[1], score[2], guesses[1], random_numbers[round]);
	}

	else
	{
		score[2]++;
		printf("3. thread win, Score: %d - %d - %d, %d is closest to %d \n--\n--\n", score[0], score[1], score[2], guesses[2], random_numbers[round]);
	}

}

//to converting 15x5 arrays value we need substring method.
void substring(char* source, char* dest, int begin, int len) {
	for (int i = begin; i < len + 1; i++)
		dest[i - 2] = source[i];
}

//Finding the winner(s)
void resulting() {

	int max_score = 0;
	for (int i = 0; i < 3; i++)
		if (max_score < score[i])
			max_score = score[i];

	int and_flag = 0;
	for (int i = 0; i < 3; i++)
		if (max_score == score[i])
		{
			if (and_flag)
				printf("and ");
			printf("%d.Thread ", i + 1);
			and_flag = 1;
		}
	printf("won the game with score: Score: %d – %d – %d.\n--\n--\n", score[0], score[1], score[2]);

}
