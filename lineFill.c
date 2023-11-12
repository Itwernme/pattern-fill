#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "cbmp.h"

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef char bool;

const uint limit = 10000000;
const char offsetPatterns[4][5][2] = {{{-1, 1}, {-1, 2}, {0, 2}, {1, 2}, {1, 1}},
																			{{1, -1}, {2, -1}, {2, 0}, {2, 1}, {1, 1}},
																			{{-1, -1}, {-1, -2}, {0, -2}, {1, -2}, {1, -1}},
																			{{-1, -1}, {-2, -1}, {-2, 0}, {-2, 1}, {-1, 1}}};
const char dirPatterns[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

BMP* bmp;

// Distance color stuff
ushort TwoUCharsToUShort(uchar a, uchar b){
	return (((ushort)a) << 8) | (0x00ff & b);
}

void UShortToTwoUChars(uchar* a, uchar* b, ushort in){
	*b = in; // to copy the first 8 bits.
	in = in >> 8; //push the other 8 bits to the right
	*a = in;
}

// The rest:

uchar dirRotated(uchar direction, char rotation){
	while (rotation < 0){
		rotation += 4;
	}
	return (direction + rotation) % 4;
}

bool checkMove(uchar dir, uint x, uint y){
	uchar hits = 0;
	for (uchar posIndex = 0; posIndex < 5; posIndex++) {
		hits += (get_pixel_r(bmp, x + offsetPatterns[dir][posIndex][0], y + offsetPatterns[dir][posIndex][1]) > 0) ? 1 : 0;
	}
	hits += (get_pixel_r(bmp, x + dirPatterns[dir][0], y + dirPatterns[dir][1]) > 0) ? 1 : 0;
	return (hits > 0) ? 0 : 1;
}

uchar findBacktrack(ushort currentDistance, uint x, uint y){
	uchar r, g, b;
	ushort arb, distance;
	for (uchar dir = 0; dir < 4; dir++) {
		get_pixel_rgb(bmp, x + dirPatterns[dir][0], y + dirPatterns[dir][1], &r, &g, &b);
		distance = TwoUCharsToUShort(g, b);
		arb = currentDistance - 1;
		if (distance == arb){
			return dir;
		}
	}
	printf("distance: %d vs %d : %d\n", arb, distance, r);
	fprintf(stderr, "stranded\n");
	return 10;
}

int main(int argc, char** argv){
	// Set random seed
	if (argc == 6){
		srand(atoi(argv[5]));
	} else {
		srand(time(NULL));
	}

	// Verify inputs
  if (argc != 5 && argc != 6){
    fprintf(stderr, "Usage: %s <input file> <output file> <[uint] startx> <[uint] starty> <(optional) [int] seed>\n", argv[0]);
    exit(1);
  }

  // Read image into BMP struct
  bmp = bopen(argv[1]);

	// Create variables
	uint startx, starty;
  uint x, y;
	ushort distance = 0;

	// Assign start values
	startx = x = atoi(argv[3]);
	starty = y = atoi(argv[4]);

	// Set Start pixel
	set_pixel_rgb(bmp, x, y, 5, 255, 0);

	// Main loop
	uint limitCounter = 0;
  while ((x != startx || y != starty || limitCounter == 0)){//&& limitCounter < limit
		limitCounter++;

		// Find possible move directions
		uchar possDirs[4];
		uchar possCount = 0;
		for (uchar dir = 0; dir < 4; dir++) {
			if (checkMove(dir, x, y) == 1){
				possDirs[possCount] = dir;
				possCount++;
			}
		}

		// Decide on move Dir
		uchar moveDir;
		bool Backtrack = 0;
		if (possCount != 0){
			moveDir = possDirs[rand() % possCount];
			distance++;
		} else {
			// Backtrack
			if (findBacktrack(distance, x, y) == 10) goto finish;
			moveDir = findBacktrack(distance, x, y);
			Backtrack = 1;
			if (distance == 0){
				distance--;
				printf("%d\n", distance);
			} else {
				distance--;
			}
		}

		// Move
		x += dirPatterns[moveDir][0];
		y += dirPatterns[moveDir][1];

		// Set pixel value
		if (!Backtrack){
			uchar g, b;
			UShortToTwoUChars(&g, &b, distance);
			set_pixel_rgb(bmp, x, y, 255, g, b);
		}
  }

finish:
		// Print end status
		if (limitCounter == limit){
			printf("limit reached!\n");
		}

		// Show finish point
		printf("finished: %d %d\n", x, y);
		set_pixel_rgb(bmp, x, y, 5, 0, 255);

	  // Write bmp contents to file
	  bwrite(bmp, argv[2]);

	  // Free memory
	  bclose(bmp);

		printf("success!\n");
	  return 0;
}