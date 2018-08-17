#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define LANDSCAPE_X_MAX 300
#define LANDSCAPE_Y_MAX 100

int map[LANDSCAPE_X_MAX][LANDSCAPE_Y_MAX];
int LANDSCAPE_X = 100;
int LANDSCAPE_Y = 100;
float VACCINATED_FRAC = 0.0;
int INITIAL_INFECTED = 0;

float get_arg(char * c, int start) {
	int i;
	char str[256] = "";
	for (i = start; i < strlen(c); i++) {
		str[i-start] = c[i];
	}
	return atof(str);
}

int parse_settings(int argc, char*argv[]) {
	int i;
	for (i = 1; i < argc; i++) {
		switch (argv[i][0]) {
			case 'l': if (argv[i][1] == 'x') { LANDSCAPE_X = (int) get_arg(argv[i], 2); if (LANDSCAPE_X > LANDSCAPE_X_MAX) {printf("Grid too large.\n"); exit(0);}} else if (argv[i][1] == 'y') { LANDSCAPE_Y = (int) get_arg(argv[i], 2); if (LANDSCAPE_Y > LANDSCAPE_Y_MAX) { printf("Grid too large.\n"); exit(0); }} else { printf("Unrecognised command\n");} break;
			case 'v': VACCINATED_FRAC = (float) get_arg(argv[i], 1); break;
			case 'i': INITIAL_INFECTED = (int) get_arg(argv[i], 1); break;
		}
	}
	return 1;
}

int main(int argc, char*argv[]){
	srand(time(NULL));
	parse_settings(argc, argv);
	printf("Welcome...\n");

	printf("Grid size: %d, %d (%d members).\n", LANDSCAPE_X, LANDSCAPE_Y, LANDSCAPE_X * LANDSCAPE_Y);
	printf("Initial conditions: %dV, %dI.\n", (int) (VACCINATED_FRAC*LANDSCAPE_X*LANDSCAPE_Y), INITIAL_INFECTED);
	return 1;
}
