//
// Tomás Oliveira e Silva, AED, December 2015
//
// union-find
//

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

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
  
  // Allocate full memory for all of the squares
  b->squares = (square *)malloc((size_t)b->n_squares * sizeof(square));
  assert(b->squares != NULL);
  
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

int find_representative(board *b,int square_number)
{
  int i,j,k;

  // find
  for(i = square_number;i != b->squares[i].representative;i = b->squares[i].representative)
    ;
  // path compression
  for(j = square_number;j != i;j = k)
  {
    k = b->squares[j].representative;
    b->squares[j].representative = i;
  }
  return i;
}

int flip_square(board *b,int square_number)
{
/*
  int fi,ti;
  edge *e;

  assert(from >= 0 && from < g->n_vertices && to >= 0 && to < g->n_vertices && from != to);
  for(e = g->vertices[from].out_edges;e != NULL && e->vertex_number != to;e = e->next)
    ;
  if(e != NULL)
    return 0;
  e = create_edge();
  e->next = g->vertices[from].out_edges;
  g->vertices[from].out_edges = e;
  e->vertex_number = to;
  e->weight = weight;
  
  fi = find_representative(g,from);
  ti = find_representative(g,to);
  if(fi != ti)
  { // union
    g->vertices[ti].representative = fi;
    g->n_connected_components--;
  }
*/
  //Turn the square
  b->squares[square_number].turned = 1;
  b->squares[square_number].representative = square_number;
  b->n_flipped++;
  
  //Find if can join something
  int top = square_number-(b->width+1);
  int bottom = square_number+(b->width+1);
  int left = square_number-1;
  int right = square_number+1;
  
  if(b->squares[top].turned == 1){
  
  }
  
  return 1;
}

board *init_board(int w, int h)
{
  board * b;
  int rnd, top, bottom, i;
  
  b = create_board(w,h);
  
  int min = b->width+2; //Do not touch the first row
  int max = b->n_squares-(b->width+1); // Do not touch the last row
  rnd = 0;
  
  for(i = 0;i < 50;i++)
  //for(;;)
  {
	rnd = rand_int(max,min,rnd); //Generate random number within the limits of the board area
	
	if(rnd % (b->width+1) != 0 && b->squares[rnd].turned != 1){ //Dont touch the first row and if the square isn't already flipped
	  printf("Flipping square #%d\n",rnd);
	  if(flip_square(b,rnd)){
	    top = find_representative(b,0);
	    bottom = find_representative(b,b->n_squares);
			
	    if(top == bottom){
	      return b;
	    }
	  }
	}
  }
  return b;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// main program
//

int main(void)
{
  //Variables
  board *b;
  int width = 10;
  int height = 20;
  int size = width*height;

  //Starting the simulation
  printf("Creating a board with dimentions of %d X %d and running the simulation..\n",width,height);
  b = init_board(width, height);
  
  //Calculate the result and display
  float p = b->n_flipped/size;
  printf("Finished the simulation with %d squares flipped. Percolation point is %f!\n",b->n_flipped,p);
  
  //Cleaning up
  destroy_board(b);
  
  return 0;
}
