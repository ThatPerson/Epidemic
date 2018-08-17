#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define LANDSCAPE_X_MAX 500
#define LANDSCAPE_Y_MAX 500

float map[LANDSCAPE_X_MAX][LANDSCAPE_Y_MAX];
int LANDSCAPE_X = 100;
int LANDSCAPE_Y = 100;
float VACCINATED_FRAC = 0.0;
int INITIAL_INFECTED = 0;
float POTENCY = 1.0;
float VAC_POTENCY = 1.0;
int PERIOD = 3;
int MODE = 0;
struct Stats {
	int NUM_ALIVE;
	int NUM_INFECTED;
	int NUM_VACCINATED;
	int NUM_DEAD;
};

float get_arg(char * c, int start) {
	int i;
	char str[256] = "";
	for (i = start; i < strlen(c); i++) {
		str[i-start] = c[i];
	}
	return atof(str);
}

int rng(int n, int m) {
	return n + rand()%(m-n+1);
}

int parse_settings(int argc, char*argv[]) {
	int i;
	for (i = 1; i < argc; i++) {
		switch (argv[i][0]) {
			case 'l': if (argv[i][1] == 'x') { LANDSCAPE_X = (int) get_arg(argv[i], 2); if (LANDSCAPE_X > LANDSCAPE_X_MAX) {printf("Grid too large.\n"); exit(0);}} else if (argv[i][1] == 'y') { LANDSCAPE_Y = (int) get_arg(argv[i], 2); if (LANDSCAPE_Y > LANDSCAPE_Y_MAX) { printf("Grid too large.\n"); exit(0); }} else { printf("Unrecognised command\n");} break;
			case 'v': VACCINATED_FRAC = (float) get_arg(argv[i], 1); if (VACCINATED_FRAC > 1 || VACCINATED_FRAC < 0) { printf("Vaccinated fraction out of bounds.\n"); exit(0); } break;
			case 'i': INITIAL_INFECTED = (int) get_arg(argv[i], 1); break;
			case 'p': if (argv[i][1] == 'i') { POTENCY = (float) get_arg(argv[i], 2); } else if (argv[i][1] == 'v') { VAC_POTENCY = (float) get_arg(argv[i], 2); } break;
			case 'd': PERIOD = (int) get_arg(argv[i], 1); break;
			case 'm': MODE = (int) get_arg(argv[i], 1); break;
			default: printf("Unrecognised command\n"); break;
		}
	}
	return 1;
}

void output_map(void) {
	int x, y;
	for (y = 0; y < LANDSCAPE_Y; y++) {
		for (x = 0; x < LANDSCAPE_X; x++) {
			switch ((int)(map[x][y])) {
				case 0: printf("."); break; //living
				case 1: printf("I"); break; //infected
				case 2: printf("-"); break; //vaccinated
				case 3: printf("#"); break; //newlyinfected
				case 4: printf("x"); break; //dead
			}
		}
		printf("\n");
	}
	return;
}

struct Stats run(void) {
	struct Stats ret;
	ret.NUM_ALIVE = 0;
	ret.NUM_INFECTED = 0;
	ret.NUM_VACCINATED = 0;
	ret.NUM_DEAD = 0;

	int x, y, min_x, min_y, max_x, max_y, xi, yi;
	for (x = 0; x < LANDSCAPE_X; x++) {
		for (y = 0; y < LANDSCAPE_Y; y++) {
			if ((int) map[x][y] == 1) {
				// lets infect some people.
				map[x][y] += 0.001;
				if (1000*(map[x][y] - (int)map[x][y]) > POTENCY)
					map[x][y] = 4;
				min_x = (x - 1 < 0)?0:x-1;
				max_x = (x + 1 >= LANDSCAPE_X)?LANDSCAPE_X-1:x+1;
				min_y = (y - 1 < 0)?0:y-1;
				max_y = (y + 1 >= LANDSCAPE_Y)?LANDSCAPE_Y-1:y+1;
				for (xi = min_x; xi <= max_x; xi++) {
					for (yi = min_y ; yi <= max_y; yi++) {
						if (xi != x || yi != y) {
							if (map[xi][yi] == 0) {
								// Infecting unvaccinated
								if (rng(0, 1000) <= POTENCY * 1000)
									map[xi][yi] = 3;
							} else if (map[xi][yi] == 2) { // Can infect vaccinated people, but multiplied by 1-vaccinepotency.
								if (rng(0, 1000) <= POTENCY * 1000 * (1 - VAC_POTENCY))
									map[xi][yi] = 3;
							}
						}
					}
				}
			}
		}
	}
	for (x = 0; x < LANDSCAPE_X; x++) {
		for (y = 0; y < LANDSCAPE_Y; y++) {
			if (map[x][y] == 3)
				map[x][y] = 1;
			switch ((int) map[x][y]) {
				case 0: ret.NUM_ALIVE++; break;
				case 1: ret.NUM_INFECTED++; break;
				case 2: ret.NUM_VACCINATED++; ret.NUM_ALIVE++; break;
				case 4: ret.NUM_DEAD++; break;
			}
		}
	}
	return ret;
}

struct Stats counter(void) {
	int x,y;
	struct Stats ret;
	ret.NUM_ALIVE = 0;
	ret.NUM_INFECTED = 0;
	ret.NUM_VACCINATED = 0;
	ret.NUM_DEAD = 0;
	for (x = 0; x < LANDSCAPE_X; x++) {
		for (y = 0; y < LANDSCAPE_Y; y++) {
			switch ((int) map[x][y]) {
				case 0: ret.NUM_ALIVE++; break;
				case 1: ret.NUM_INFECTED++; break;
				case 2: ret.NUM_VACCINATED++; ret.NUM_ALIVE++; break;
				case 4: ret.NUM_DEAD++; break;
			}
		}
	}
	return ret;
}

int main(int argc, char*argv[]){
	srand(time(NULL));
	parse_settings(argc, argv);
	printf("Welcome...\n");

	printf("Grid size: %d, %d (%d members).\n", LANDSCAPE_X, LANDSCAPE_Y, LANDSCAPE_X * LANDSCAPE_Y);

	printf("Initial conditions: %dV, %dI.\n", (int) (VACCINATED_FRAC*LANDSCAPE_X*LANDSCAPE_Y), INITIAL_INFECTED);
	if (INITIAL_INFECTED > LANDSCAPE_X*LANDSCAPE_Y * (1-VACCINATED_FRAC)) {
		printf("> Too many initial infected. Reducing number of infected. \n");
		INITIAL_INFECTED = (LANDSCAPE_X*LANDSCAPE_Y * (1-VACCINATED_FRAC)) - 5;
		printf("Initial conditions: %dV, %dI.\n", (int) (VACCINATED_FRAC*LANDSCAPE_X*LANDSCAPE_Y), INITIAL_INFECTED);
	}

	printf("Generating map...\n");
	int x, y;
	for (x = 0; x < LANDSCAPE_X; x++) {
		for (y = 0; y < LANDSCAPE_Y; y++) {
			if (rng(0, 1000) < VACCINATED_FRAC * 1000) {
				map[x][y] = 2; // vaccinated
			} else {
				map[x][y] = 0; //unvaccinated
			}
		}
	}
	int n = 0;
	int NUM_INFECTED = 0;
	while (NUM_INFECTED < INITIAL_INFECTED && n < 10000) {
		x = rng(0, LANDSCAPE_X - 1);
		y = rng(0, LANDSCAPE_Y - 1);
		if (map[x][y] == 0) {
			map[x][y] = 1;
			NUM_INFECTED++;
		} else {
			n++;
		}
	}

	printf("Infected %d\n", NUM_INFECTED);
	n = 0;
	struct Stats s;
	printf("INITIAL MAP\n");
	s = counter();
	printf("%d\t%d\t%d\t%d\n", s.NUM_ALIVE, s.NUM_INFECTED, s.NUM_VACCINATED, s.NUM_DEAD);
	output_map();

	while (s.NUM_INFECTED > 0 && n < 10000) {
		s = run();
		switch (MODE) {
			case 1: printf("%d\t%d\t%d\t%d\t%d\n", s.NUM_ALIVE, s.NUM_INFECTED, s.NUM_VACCINATED, s.NUM_DEAD, s.NUM_ALIVE - s.NUM_VACCINATED); break;
			case 2: printf("\n\n%d\n", n); output_map(); break;
			default: printf("%d\t%d\t%d\t%d\t%d\n", s.NUM_ALIVE, s.NUM_INFECTED, s.NUM_VACCINATED, s.NUM_DEAD, s.NUM_ALIVE - s.NUM_VACCINATED); printf("\n\n"); output_map(); break;
		}
		n++;
	}
	return 1;
}
