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
#include <math.h>

int random_number();
void guess(int array[]);
void comparasion(int* score_p, int* score_c, int randoms[], int guess_p[], int guess_c[]);

int main() {
	//Creating pipe to start IPC
	int pipefds[2];
	int returnstatus;

	returnstatus = pipe(pipefds);

	if (returnstatus == -1) {
		printf("Unable to create pipe \n");
		return 1;
	}

	//Defining arrays to store random generated values.
	int random_numbers[5];
	int child[5];
	int parent[5];

	srand(time(NULL));
	guess(random_numbers);

	//Running the game.
	printf("The game has launched\n");
	printf("Computer choses [");
	for (int i = 0; i < 5; i++)
		printf("%d, ", random_numbers[i]);

	printf("] randomly\n");
	printf("Child process is crated\n");
	printf("The game starts\n--\n");

	//Creating child process with fork.
	pid_t pid;
	pid = fork();

	srand(getpid());

	// If pid less than 0 that means fork is failed.
	if (pid < 0) {
		printf("Fork is failed \n");
		return 1;
	}

	// Child process
	else if (pid == 0) {
		close(pipefds[0]);
		guess(child);

		//Transfers the random numbers via IPC
		for (int i = 0; i < 5; i++)
			write(pipefds[1], &child[i], sizeof(child[i]));

		//Terminate the child process when it's done.
		exit(NULL);

	}

	// Parent process
	else {
		guess(parent);

		//Recieves the random numners via IPC
		for (int i = 0; i < 5; i++)
			read(pipefds[0], &child[i], sizeof(child[i]));

		int score_p = 0;
		int score_c = 0;

		comparasion(&score_p, &score_c, random_numbers, parent, child);

		if (score_p > score_c)
			printf("Parent has won the game with score: %d - %d \n", score_p, score_c);
		else if (score_p < score_c)
			printf("Child has won the game with score: %d - %d \n", score_p, score_c);
		else
			printf("Draw with score: %d - %d \n", score_p, score_c);

		printf("Child has terminated\n");
		printf("Parent waits child with wait()\n");

		wait(NULL);

		printf("Parent has terminated\n");
	}

	return 0;
}

//Creates random numbers
int random_number() {
	return rand() % 99 + 1;
}

// Fills the given array with the random numbers
void guess(int array[5]) {
	for (int i = 0; i < 5; i++)
		array[i] = random_number();
}

//Makes the comparassion between Child process and Parent process to find closest one to computer.
void comparasion(int* score_p, int* score_c, int randoms[], int guess_p[], int guess_c[]) {

	for (int i = 0; i < 5; i++) {
		printf("Turn %d, Parent guess: %d, Child guess: %d \n", i + 1, guess_p[i], guess_c[i]);

		if (abs(randoms[i] - guess_p[i]) < abs(randoms[i] - guess_c[i])) {
			(*score_p)++;
			printf("Parent win, Score: %d - %d, %d is closer to %d than %d \n--\n--\n", *score_p, *score_c, guess_p[i], randoms[i], guess_c[i]);
		}

		else if (abs(randoms[i] - guess_p[i]) > abs(randoms[i] - guess_c[i])) {
			(*score_c)++;
			printf("Child win, Score: %d - %d, %d is closer to %d than %d \n--\n", *score_p, *score_c, guess_c[i], randoms[i], guess_p[i]);
		}

		else
			printf("Tie, both got equal distance to target value..  \n--\n");
	}

}
