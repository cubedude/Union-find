//
// Tomás Oliveira e Silva, AED, December 2015
//
// union-find
//

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#define VERBOSE           0

////////////////////////////////////////////////////////////////////////////////////////////////////
// custom graph implementation (with union-find infrastructure)
//

inline int rand_int(int upper, int lower, int prev){
  //Trying to randomise the seed with more random values and previous result
  srand ( time(NULL)+rand()+prev);
  return((rand() % (upper-lower+1)) + lower);
}

typedef struct square
{
  int turned;         // to check if it is turned
  int representative;     // representative of each connected component
}
square;

typedef struct board
{
  struct square *squares;    // array of squares (pointer)
  int width;                 // Width of the play ares
  int height;                // Height of the play area
  int n_squares;             // number of squares
  int n_flipped;             // number of squares flipped
}
board;

board *create_board(int w, int h)
{
  board *b;
  int i;
    
  // Allocate memory for the board
  b = (board *)malloc(sizeof(board));
  assert(b != NULL);
  
  // Define how many squares there are including the system squares
  b->width = w;
  b->height = h;
  b->n_squares = (w+1)*(h+2);
  b->n_flipped = 0;
  
  // Allocate full memory for all of the squares
  b->squares = (square *)malloc((size_t)b->n_squares * sizeof(square));
  assert(b->squares != NULL);
  
  //Visit every node and reset the data
  for(i = 0;i < b->n_squares;i++)
  {
	  b->squares[i].turned = 0;
	  b->squares[i].representative = i;
  }
  
  // Turn the most top and bottom rows into connected components
  for(i = 0;i < (b->width+1);i++)
  {
	  // Top row
	  b->squares[i].turned = 1;
	  b->squares[i].representative = 0;
	  
	  // Bottom row
	  int bottom = b->n_squares-i;
	  b->squares[bottom].turned = 1;
	  b->squares[bottom].representative = b->n_squares;
  }
  
  return b;
}

void destroy_board(board *b)
{
  // Free the memory
  free(b->squares);
  free(b);
}

void change_representative(board *b,int from, int to)
{
  //Go through every square
  for(int i = 0;i < b->n_squares;i++){
	//Check if it belongs under the group thats gonna get joined
	if(b->squares[i].representative == from){
	  //Change the representative to the new group
	  b->squares[i].representative = to; 
	}
  }
}

int flip_square(board *b,int square_number)
{
  //Turn the square
  b->squares[square_number].turned = 1;
  b->squares[square_number].representative = square_number;
  b->n_flipped++;
  
  //Find if can join something
    
  //The top
  int top = square_number-(b->width+1);
  if(b->squares[top].turned == 1){
	change_representative(b,b->squares[square_number].representative,b->squares[top].representative);
  }
  
  //The bottom
  int bottom = square_number+(b->width+1);
  if(b->squares[bottom].turned == 1){
	change_representative(b,b->squares[square_number].representative,b->squares[bottom].representative);
  }
  
  //The left
  int left = square_number-1;
  if(b->squares[left].turned == 1){
	change_representative(b,b->squares[square_number].representative,b->squares[left].representative);
  }
  
  //The right
  int right = square_number+1;
  if(b->squares[right].turned == 1){
	change_representative(b,b->squares[square_number].representative,b->squares[right].representative);
  }
    
  return 1;
}

board *init_board(int w, int h)
{
  board * b;
  int rnd, top, bottom;
  
  b = create_board(w,h);
  
  int min = b->width+2; //Do not touch the first row
  int max = b->n_squares-(b->width+1); // Do not touch the last row
  rnd = 0;
  
  //for(int i = 0;i < 500000;i++)
  for(;;)
  {
	rnd = rand_int(max,min,rnd); //Generate random number within the limits of the board area
	
	if(rnd % (b->width+1) != 0 && b->squares[rnd].turned != 1){ //Dont touch the first row and if the square isn't already flipped
  #if VERBOSE != 0
	  printf("Flipping square #%d\n",rnd);
  #endif
	  if(flip_square(b,rnd)){
	    top = b->squares[0].representative;
	    bottom = b->squares[(b->n_squares-b->width)].representative;
  #if VERBOSE != 0
		printf("Top: %d, Bottom: %d\n",top,bottom);
  #endif
	    if(top == bottom){
  #if VERBOSE != 0
		  printf("BREAK!\n");
  #endif
	      break;
	    }
	  }
	}
	else{
	  //printf("Square #%d is already flipped or not valid\n",rnd);
	}
  }
  return b;
}

int run_simulation(int width, int height, int verbose)
{
  //Variables
  board *b;
  int size = width*height;

  //Starting the simulation
  if(verbose)
    printf("Creating a board with dimentions of %d X %d and running the simulation..\n",width,height);
    
  b = init_board(width, height);
  
  //Calculate the result and display
  if(verbose)
    printf("Finished the simulation with %d squares flipped out of %d!\n",b->n_flipped,size);
  
  if(verbose == 2){
	  int i,j;
	  int n = 0;
	  
	  for(j = 0;j < (height+2);j++){
		for(i = 0;i < (width+1);i++){
		  if(b->squares[n].turned == 1){
			printf("|%-3d",b->squares[n].representative); 
		  }
		  else{
			printf("|   "); 
		  }
		  n++;
		}
		printf("|\n");
		for(i = 0;i < (width+1);i++){
			printf("----"); 
		}
		printf("|\n"); 
	  }  
  }
  
  int changed = b->n_flipped;
  
  //Cleaning up
  destroy_board(b);
  
  return changed;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// main program
//

int main(void)
{
  //Variables
  int i;
  int times = 3;
  
  int width = 20;
  int height = 10;
  int size = width*height;
  
  int verboseSim = 1; // 0 - off, 1 - on, 2 - display table aswell

  printf("Creating a board with dimentions of %d X %d and running the simulation %d times..\n",width,height,times);
  
  //Opening the file to save results into
  FILE *f = fopen("Results.txt", "w");
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }
  
  //Run the simulations
  int result;
  for(i = 0;i < times;i++){
    result = run_simulation(width, height, verboseSim);
    
    // Output the results
	fprintf(f, "Run #%d: %d / %d\n", (i+1), result, size);
	printf("Run #%d: %d / %d\n", (i+1), result, size);
  }
  
  printf("Completed %d runs and saved the results into \"Results.txt\" file...\n",i);
  fclose(f);
  
  return 0;
}
