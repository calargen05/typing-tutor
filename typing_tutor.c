#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

void set_terminal_mode(int enable);
void processing_timer(clock_t start, clock_t end, int duration);
void countdown(clock_t start, clock_t end, int duration);
double _speed(int correct, double time);
double _accuracy(int correct, int total_chars);
int _score(int correct);


int main(int argc, char** argv)
{
	// the length_of_test can change based on... well, that's self-explanatory
	int length_of_test = 500;
	FILE* file_ptr1;
	char output[length_of_test];
	char input[length_of_test];
	char in;
	int time;
	int iteration=0;
	int chars=0;
	double speed;
	double accuracy;
	int score;
	int total_chars=0;
	int ptime=10;

	printf("Welcome to the typing test!\n");

	file_ptr1 = fopen("typing_from_file.txt", "r");

	if (file_ptr1 == NULL) {
		printf("Error opening the typing file");
		exit(1);
	}
	
	printf("Would you like 15 seconds to type or 30 seconds?: ");
	scanf("%d", &time);
	if (time != 15 && time != 30) {
		printf("Invalid time choice. Exiting typing tutor.\n");
		return 1;
	}
	else {
		printf("\nOk. Take a look at the text you'll be typing:\n\n");
		if (fgets(output, length_of_test, file_ptr1)) {
			puts(output);
		}
		fclose(file_ptr1);

		// DAY 2: MOVE THE PRINT_TIMER FUNCTION INTO THE MAIN FUNCTION BECAUSE THE TIMER WILL ONLY WORK WHEN IT'S DEFINED AND MADE IN THE MAIN FUNCTION
		// UPDATE: 4 months later, I figured it out, just use fflush to output each number after each second to output it on the same line :)
		// countdown function
		clock_t start = clock();
		clock_t end;
		countdown(start, end, ptime);

		// enable non-canon and non blocking
		set_terminal_mode(1);

		printf("\nStart Typing: ");
		fflush(stdout);
		
		// DAY 1: TRY TO FIND A WAY TO MAKE THE SCANF AND THE NONPRINT_TIMER FUNCTIONS TO RUN SIMULTANEOUSLY
		// UPDATE: 4 months later, I figured it out, just record the clock() before and constantly check if the time is less than the allotted time
		
		// start of recording time and input from user
		clock_t start_time = clock();
		clock_t end_time;
		float elapsed_time = 0;

		while (elapsed_time < time) {
			end_time = clock();
			elapsed_time = (float)(end_time - start_time)/CLOCKS_PER_SEC;

			if (read(STDIN_FILENO, &in, 1) > 0) {
				if (in == 127) { // if backspace key is pressed
					--iteration;
					printf("\b \b"); // move back, overwrite, and move back again
					fflush(stdout);
				}
				else {
					input[iteration++] = in;
					printf("%c", in);
					fflush(stdout);
				}
			}
		}

		input[iteration] = '\0';

		printf("\nTIME'S UP!\n\n\n");

		set_terminal_mode(0);
		
		start = clock();
		end = 0;
		processing_timer(start, end, 3);


		for (iteration=0; input[iteration]!='\0'; iteration++) {
			if (input[iteration]==output[iteration]) {
				chars++;
				total_chars++;
			}
			else
				total_chars++;
		}

		accuracy = _accuracy(chars,total_chars);
		speed = _speed(chars,time);
		score = _score(chars);

		printf("\nYour accuracy: %.2f%%\n", accuracy);
		printf("Your speed: %.2f chars/sec\n", speed);
		printf("Your score: %d\n\nThanks for playing. Goodbye\n", score);
	}

	// MAKE SURE TO CLEAR THE INPUT STREAM AFTER THE TYPING IS DONE!!
	
	return 0;

}

double _speed(int correct, double time)
{
	return correct/time;
}

double _accuracy(int correct, int total_chars)
{
	double result = ((double)correct/total_chars)*100;
	return result;
}

int _score(int correct)
{
	return correct*10;
}

void countdown(clock_t start, clock_t end, int duration)
{
	while (duration > 0) {
		end = clock();
		if ((end-start) >= CLOCKS_PER_SEC) {
			--duration;
			start += CLOCKS_PER_SEC;
			if (duration > 0) {
				printf("%d... ", duration);
				fflush(stdout);
			}
			else {
				break;
			}
		}
	}
}

void processing_timer(clock_t start, clock_t end, int duration)
{
	printf("Calculating...\n");
	while (duration > 0) {
		end = clock();
		if ((end-start) >= CLOCKS_PER_SEC) {
			--duration;
			start += CLOCKS_PER_SEC;
		}
	}
}

void set_terminal_mode(int enable) {
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	if (enable) {
		// disabling canonical mode and echo
		t.c_lflag &= ~ICANON;
		t.c_lflag &= ~ECHO;
		tcsetattr(STDIN_FILENO, TCSANOW, &t);

		int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
		fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
	}
	else {
		// enabling canonical mode and echo
		t.c_lflag |= ICANON;
		t.c_lflag |= ECHO;
		tcsetattr(STDIN_FILENO, TCSANOW, &t);

		// restore file descriptor to blocking mode;
		int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
		fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
	}
}
