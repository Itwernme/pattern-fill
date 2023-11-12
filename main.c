#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "cbmp.h"

typedef unsigned int uint;
typedef unsigned char uchar;

enum dir{
  UP,
  RIGHT,
  DOWN,
  LEFT
};

const uint limit = 10000000;

BMP* bmp;

uchar dirRotated(uchar direction, char rotation){
	while (rotation < 0){
		rotation += 4;
	}
	return (direction + rotation) % 4;
}

char originRotation(uchar direction, uint x, uint y){
	uint state = 0;
	switch (direction){
    case 0:
			state += get_pixel_r(bmp, x-1, y)/255;
      state += get_pixel_r(bmp, x+1, y)/255 * 2;
			state += get_pixel_r(bmp, x, y-1)/255 * 4;
      break;
    case 1:
			state += get_pixel_r(bmp, x, y+1)/255;
      state += get_pixel_r(bmp, x, y-1)/255 * 2;
			state += get_pixel_r(bmp, x-1, y)/255 * 4;
      break;
    case 2:
		  state += get_pixel_r(bmp, x+1, y)/255;
      state += get_pixel_r(bmp, x-1, y)/255 * 2;
			state += get_pixel_r(bmp, x, y+1)/255 * 4;
      break;
    case 3:
		  state += get_pixel_r(bmp, x, y-1)/255;
      state += get_pixel_r(bmp, x, y+1)/255 * 2;
			state += get_pixel_r(bmp, x+1, y)/255 * 4;
      break;
  }
	printf("%d\n", state);
	char value;
	switch (state){
		case 0:
			return 0;
		case 1:
			return -1;
		case 2:
			return 1;
		case 3:
			value = rand() % 2;
			value += value - 1;
			return value;
		case 4:
			return 2;
		case 5:
			value = rand() % 2;
			value += 2 - value * 3;
			return value;
		case 6:
			value = (rand() % 2) + 1;
			return value;
		case 7:
			value = (rand() % 3);
			value += (x > 0) - (x < 0) - 1;
			return value;
	}
	exit(1);
}

uint checkMove(uint direction, uint x, uint y){
	uchar hits = 0;
  switch (direction){
    case 0:
			hits += get_pixel_r(bmp, x-1, y+1);
      hits += get_pixel_r(bmp, x-1, y+2);
      hits += get_pixel_r(bmp, x, y+2);
      hits += get_pixel_r(bmp, x+1, y+2);
			hits += get_pixel_r(bmp, x+1, y+1);
      break;
    case 1:
			hits += get_pixel_r(bmp, x+1, y-1);
      hits += get_pixel_r(bmp, x+2, y-1);
      hits += get_pixel_r(bmp, x+2, y);
      hits += get_pixel_r(bmp, x+2, y+1);
			hits += get_pixel_r(bmp, x+1, y+1);
      break;
    case 2:
		  hits += get_pixel_r(bmp, x-1, y-1);
      hits += get_pixel_r(bmp, x-1, y-2);
      hits += get_pixel_r(bmp, x, y-2);
      hits += get_pixel_r(bmp, x+1, y-2);
			hits += get_pixel_r(bmp, x+1, y-1);
      break;
    case 3:
		  hits += get_pixel_r(bmp, x-1, y-1);
      hits += get_pixel_r(bmp, x-2, y-1);
      hits += get_pixel_r(bmp, x-2, y);
      hits += get_pixel_r(bmp, x-2, y+1);
			hits += get_pixel_r(bmp, x-1, y+1);
      break;
  }
	//printf("hits: %d\n", hits);
	return hits;
}

int main(int argc, char** argv){
	// Set random seed
	srand(time(NULL));

	// Verify inputs
  if (argc != 5){
    fprintf(stderr, "Usage: %s <input file> <output file> <[uint] startx> <[uint] starty>\n", argv[0]);
    exit(1);
  }

  // Read image into BMP struct
  bmp = bopen(argv[1]);

	//uint width, height;
	//width = get_width(bmp);
  //height = get_height(bmp);

	// Create variables
	uint startx, starty;
  uint x, y;
	uchar facing;

	startx = x = atoi(argv[3]);
	starty = y = atoi(argv[4]);

  // main loop
	printf("starting loop\n");
	uint limitCounter = 0;
  while ((x != startx || y != starty || limitCounter == 0) && limitCounter < limit){//&& limitCounter < limit
		limitCounter++;
		//printf("pos: %d, %d\n", x, y);

		char turn = (rand() % 3) - 1;

		uchar target = dirRotated(facing, turn);
		//printf("turn: %d\n", turn);
		//printf("target: %d\n", target);

		while (checkMove(target, x, y) != 0 && target != dirRotated(facing, 2)){
			target = dirRotated(target, turn + 2);
			//printf("pos: %d, %d\n", x, y);
			//printf("target: %d\n", target);
		}
		//printf("target: %d\n", target);
		//if (checkMove(target, x, y) != 0){
			//target = dirRotated(target, 2);
			//printf("o target: %d\n", target);
			//if (checkMove(target, x, y) != 0){
				//printf("oo target: %d\n", target);
				//target = dirRotated(facing, 2);
			//}
		//}

		// Rotate
		if (target != dirRotated(facing, 2)){
			facing = target;
		} else {
			char value = originRotation(facing, x, y);
			if (value == 0){
				printf("stuck\n");
				goto finish;
			}
			facing = dirRotated(facing, value);
		}

		// Move
		switch (target){
	    case 0:
	      y += 1;
	      break;
	    case 1:
	      x += 1;
	      break;
	    case 2:
	      y -= 1;
	      break;
	    case 3:
	      x -= 1;
	      break;
	  }

		if (facing == target){
			set_pixel_rgb(bmp, x, y, 255, 255, 255);
		} else {
			set_pixel_rgb(bmp, x, y, 255, 0, 0);
		}
  }

finish:
	// Print end status
	if (limitCounter == limit){
		printf("limit reached!\n");
	}
	printf("finish pos: %d, %d\n", x, y);
	set_pixel_rgb(bmp, x, y, 0, 255, 255);

	//get_pixel_rgb(bmp, y, x, &r, &g, &b);
  //set_pixel_rgb(bmp, x, y, 255 - r, 255 - g, 255 - b);
  // Write bmp contents to file
  bwrite(bmp, argv[2]);

  // Free memory
  bclose(bmp);

	printf("done\n");
  return 0;
}
