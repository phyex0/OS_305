/*

	Halit Burak Yeşildal		18050111043
	Ayşe Tüncer					17050111058
	Zeynep Nur Tüncel			17050111011
	Yakup Batuhan Ördek			18050111041
	Berna Altun					16050111040
	Enes Güler					18050111005

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

//global variable which is allowed to access by all threads.
int guess_by_thread[3][5];

int random_number();
void guess(int array[]);
void *runner(void *param);
void comparasion(int score[], int randoms[]);

int main()
{

	srand(time(NULL));

	//computer made its own decision.
	int random_numbers[5];
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
	pthread_t tid[3];

	//creating threads.
	for (int i = 0; i < 3; i++)
		pthread_create(&tid[i], NULL, runner, guess_by_thread[i]);

	//terminating the threads.
	for (int i = 0; i < 3; i++)
		pthread_join(tid[i], NULL);

	int score[3] = {0, 0, 0};
	comparasion(score, random_numbers);

	//resulting
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

	printf("Guesses made by threads: [");
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 5; j++)
			printf("%d_%d, ", i + 1, guess_by_thread[i][j]);
	}
	printf("] \n --\n--\n");

	for (int i = 1; i <= 3; i++)
		printf("%d. Thread terminated\n", i);

	printf("Threads are joined by main process \nGame Finished\n");

	return 0;
}

//runner method of theads.
void *runner(void *param)
{
	//type casting for the integer array.
	int *i = (int *)param;

	guess(i);
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

//Makes the comparassion for the guesses of theads.
void comparasion(int score[], int randoms[])
{

	for (int i = 0; i < 5; i++)
	{
		printf("Turn %d, Guesses: 1.thread: %d, 2:thread: %d, 3:thread: %d \n", i + 1, guess_by_thread[0][i], guess_by_thread[1][i], guess_by_thread[2][i]);

		if (abs(randoms[i] - guess_by_thread[0][i]) < abs(randoms[i] - guess_by_thread[1][i]) && abs(randoms[i] - guess_by_thread[0][i]) < abs(randoms[i] - guess_by_thread[2][i]))
		{
			score[0]++;
			printf("1. thread win, Score: %d - %d - %d, %d is closest to %d \n--\n--\n", score[0], score[1], score[2], guess_by_thread[0][i], randoms[i]);
		}

		else if (abs(randoms[i] - guess_by_thread[1][i]) < abs(randoms[i] - guess_by_thread[0][i]) && abs(randoms[i] - guess_by_thread[1][i]) < abs(randoms[i] - guess_by_thread[2][i]))
		{
			score[1]++;
			printf("2. thread win, Score: %d - %d - %d, %d is closest to %d \n--\n--\n", score[0], score[1], score[2], guess_by_thread[1][i], randoms[i]);
		}

		else
		{
			score[2]++;
			printf("3. thread win, Score: %d - %d - %d, %d is closest to %d \n--\n--\n", score[0], score[1], score[2], guess_by_thread[2][i], randoms[i]);
		}
	}
}
