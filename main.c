#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "uthash.h"

int int_rows;
int int_cols;

// map for storing matrices
struct my_struct {
    UT_hash_handle hh; /* makes this structure hashable */
    int id;            /* we'll use this field as the key */
    int column;
    int row;
    int *matrix_mem;
};

struct my_struct *saved_matrices = NULL;


void save_matrix(struct my_struct *s) {
    HASH_ADD_INT( saved_matrices, id, s );
}

struct my_struct *find_matrix(int id) {
    struct my_struct *s;
    HASH_FIND_INT( saved_matrices, &id, s );
    return s;
}

void printMatrixGlobalDim(int *given_matrix){//internally uses latest matrix dimensions
    for(int i = 0; i < int_rows; i++){ 
       for(int j = 0; j < int_cols; j++){
          printf("%d\t", given_matrix[i * int_cols + j]);
       }
       printf("\n"); 
   }
}

void printMatrixGivenDim(int *fetched_matrix, int rows, int cols){//iterates based on given arguments
    for(int i = 0; i < rows; i++){ 
       for(int j = 0; j < cols; j++){
          printf("%d\t", fetched_matrix[i * cols + j]);
       }
       printf("\n"); 
   }
}

void setPointValue(int value, int row, int col, int rowSize, int colSize, int** matrix){
  (*matrix)[row * int_cols + col] = value; //must pass dimensions rowSize colSize, otherwise C cannot determine where to place value as the array would then be varriable length array (VLA). So pointer arithmetic would fail
}

void fillValues(int value, int rows, int cols, int** matrix){
  for (int i = 0; i < int_cols*int_rows; i++){ 
	   (*matrix)[i] = value;
   } 
}

bool validateInput(char** input){
  return true;//TODO (0 < input || input  < int_rows -1) && (0 < input || input < int_cols-1);
}

void resizeMatrix(int rowDim, int colDim, int preRowDim, int preColDim, int** matrix){ //pass a pointer to the pointer, otherwise you will only modify local copy and caller points to old memory block, (make sure functions before this forward the pointer as ** otherwise you risk only having a copy of the pointer) 
//If shrinking, it simply marks the unused tail as free
//If moving, it copies the old data into a new smaller block
//You never free individual elements
//You only free the whole matrix once at the end of the program

   int oldSize = preRowDim * preColDim;
   int newSize = rowDim * colDim;

   *matrix = realloc(*matrix, newSize * sizeof(int));
 // Initialize new region
   for (int i = oldSize; i < newSize; i++){ 
	   (*matrix)[i] = 0;
   }
   int_rows = rowDim; 
   int_cols = colDim;
   
}

void newRandomMatrix(int rowDim, int colDim, int preRowDim, int preColDim, int** matrix){
   *matrix = realloc(*matrix, rowDim * colDim * sizeof(int));

   srand(time(NULL));   // Initialization, should only be called once.
   for (int i = 0; i < rowDim * colDim;  i++){ 
	   (*matrix)[i] = rand();
   }
   int_rows = rowDim; 
   int_cols = colDim;
}

void multiplyMatrixWithInteger(int value, int rows, int cols, int** matrix){
   for (int i = 0; i < int_cols*int_rows; i++){ 
	   (*matrix)[i] = value * (*matrix)[i];
   } 
}

void multiplyMatrixWithMatrix(){
   //TODO multiply matrix a (m x n) with matrix b (m x k), so the product is n x k
}

void doActionInput(char** action, int rowSize, int colSize, int* matrix, int** matrixp){
   if (strcmp(action[0], "set") == 0){
     //> set 2 2 7 //set value in row 2 column 2 to value 7
       setPointValue(atoi(action[3]), atoi(action[1]), atoi(action[2]), int_rows, int_cols, &matrix);
   } else if (strcmp(action[0], "fill") == 0){
     //> fill 3 //fill entire matrix with value 3 
      fillValues(atoi(action[1]), int_rows, int_cols, &matrix); 
   } else if (strcmp(action[0], "resize") == 0){
     //> resize 4 3 //resize existing matrix to 4 x 3
      resizeMatrix(atoi(action[1]), atoi(action[2]), int_rows, int_cols, matrixp);
   } else if (strcmp(action[0], "newrand") == 0){
     //> newrand 4 3 //make new 4 x 3 matrix with random values
     newRandomMatrix(atoi(action[1]), atoi(action[2]), int_rows, int_cols, matrixp);
   } else if (strcmp(action[0], "mult") == 0){
     //> mult 2 //multiply each value in matrix by 2
     multiplyMatrixWithInteger(atoi(action[1]), int_rows, int_cols, &matrix);
   } else if (strcmp(action[0], "save") == 0){
     //> save 2 //save matrix with with key 2
     struct my_struct *thisMatrix = malloc(sizeof(struct my_struct));

     thisMatrix->matrix_mem = malloc(int_rows * int_cols * sizeof(int));
     thisMatrix -> id = atoi(action[1]);
     thisMatrix -> column = int_cols;
     thisMatrix -> row = int_rows;
     
     printf("save: int_rows=%d int_cols=%d\n", int_rows, int_cols); 
     printf("save: matrix ptr=%p\n", (void*)matrix); 
     printf("save: matrix[0]=%d\n", matrix[0]); 
     printf("save: matrix_mem[0] before memcpy=%d\n", thisMatrix->matrix_mem[0]);
     memcpy(thisMatrix -> matrix_mem, matrix, int_rows * int_cols * sizeof(int));
     printf("save: matrix_mem[0] after memcpy=%d\n", thisMatrix->matrix_mem[0]);     
     printf("saving pointer: %p\n", (void*)thisMatrix->matrix_mem);

     printf("before save: first = %d\n", thisMatrix->matrix_mem[0]);
     save_matrix(thisMatrix);
     printf("after save: first = %d\n", thisMatrix->matrix_mem[0]);
   } else if (strcmp(action[0], "find") == 0){
     //> find 2 //find the saved matrix by looking up key 2
     struct my_struct *thisMatrix = find_matrix(atoi(action[1]));
     int *matrix_mem = thisMatrix -> matrix_mem;
     int rows = thisMatrix -> row;
     int cols = thisMatrix -> column;

     printf("before find-print: first = %d\n", thisMatrix->matrix_mem[0]);
     printf("matrix_mem = %p\n", (void*)thisMatrix->matrix_mem); 
     printf("first element = %d\n", thisMatrix->matrix_mem[0]);
     printf("found pointer: %p\n", (void*)thisMatrix->matrix_mem);
     printMatrixGivenDim(matrix_mem, rows, cols);
   }
   else {
      printf("Unrecognized function\n");
   }

}

void parseProgramInput(char** in, int rowSize, int colSize, int* matrix, int** matrixp){
  //TODO validate arg 'in' along with dimensions rowSize and colSize

  if (validateInput(in) == false){
     printf("Error incorrect input or dimension \n");
     return;
  }
  doActionInput(in, int_rows, int_cols, matrix, matrixp);   
}

char** tokenize(char in[]){ 
    char* temp = strdup(in);
    char* token = strtok(temp, " ");

    char** list = NULL;
    int count = 0;
    
    while(token != NULL){

    list = realloc(list, (count + 1) * sizeof(char*));
     list[count] = strdup(token);
     (count)++;
          token = strtok(NULL," ");
    }
    free(temp);
    return list;
}

int main(int argc, char *argv[]){

   int_rows = *argv[1] - '0'; //first matrix dimension program arg
   int_cols = *argv[2] - '0'; //second matrix dimension program arg

   int *matrix = malloc(int_rows * int_cols * sizeof(int)); //flat buffer/single contiguous block of memory
   for (int i = 0; i < int_rows * int_cols; i++){
	   matrix[i] = 0;
  }
   printf("This is graph program: Hello %s \n","User");
   printf("The Graph dimension is %i x %i\n", int_rows, int_cols);
   printf("> set 2 2 7 //set value in row 2 column 2 to value 7\n");
   printf("> fill 3 //fill entire matrix with value 3\n");
   printf("> resize 4 3 //make new 4 x 3 matrix\n");
   printf("> newrand 4 3 //make new 4 x 3 matrix with random values\n");
   printf("> mult 2 //multiply each value in matrix by 2\n");
   printf("> save 2 //save matrix with with key 2\n"); 
   printf("> find 2 //find the saved matrix by looking up key 2\n");
   while(1){
    printMatrixGlobalDim(matrix);    

    char in[100];
    scanf(" %[^\n]", in);

    char** list = tokenize(in);

    parseProgramInput(list, int_rows, int_cols, matrix, &matrix);
        
    free(list);
    //free(saved_matrices); //TODO consider where to free saved matrices

    }
}
