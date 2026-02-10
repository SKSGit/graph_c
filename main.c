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

void delete_all_matrices()
{
    struct my_struct *current_user;
    struct my_struct *tmp;

    HASH_ITER(hh, saved_matrices, current_user, tmp) {
        HASH_DEL(saved_matrices, current_user);  /* delete it (users advances to next) */
        free(current_user -> matrix_mem);
	free(current_user);             /* free it */
    }
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
					   //TODO prevent setting value out of bounds of array. C doesnt care if you mess up.
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

int product(int a, int b){
   int product = 0;
   product = a * b;
   return product;
}

int* iterateSumProductOverSndMatrixCol(int* fstMatrRow, int n1, int** matrix, int k, int n2, int* sequenceOfDotProducts){
 int sndMatrixCol[k];
 int offsetCol = 0;

int dot_product = 0;

for (int i = 0 ; i < n1; i++){
     int currentValue = (fstMatrRow)[i];

//printf("index: %d AAAH: %d\n", i,  currentValue);
}
sequenceOfDotProducts = realloc(sequenceOfDotProducts, (k));

 for (int i = 0; i < k; i++){
       offsetCol = i;
    //   printf("index i: %d\n", i);

    for (int j = offsetCol; j < n2 * k; j=j+k){
       sndMatrixCol[i] = (*matrix)[j];
    //   printf("index j: %d, sndMatrixCol: %d, matrix: %d\n", j, sndMatrixCol[i], (*matrix)[i]);

    }

    for (int f = 0; f < n1; f++){
    //   printf("fstMatrixRow: %d sndMatrixCol: %d\n", fstMatrRow[f], sndMatrixCol[i]);

       dot_product += fstMatrRow[f] * sndMatrixCol[i];
    }
  //  printf("Dot Product: %d\n", dot_product);
    sequenceOfDotProducts[i] = dot_product;
    dot_product = 0;
    
 }
for (int i = 0 ; i < k; i++){
     int currentValue = (sndMatrixCol)[i];
//printf("index: %d currentValue: %d\n", i, currentValue);
}

return sequenceOfDotProducts;
}

//multiply matrix a (m x n) with matrix b (n x k), so the product is m x k
void multiplyMatrixWithMatrix(int matrixId, int grow, int gcols, int** matrix){
   
   struct my_struct *thisMatrixToSave = find_matrix(matrixId);
   int *matrix_mem = thisMatrixToSave -> matrix_mem;
   int rows = thisMatrixToSave -> row;
   int cols = thisMatrixToSave -> column;

   //first: m x n1, 
   int m = rows;
   int n1 = cols;
   printf("m: %d, n1: %d\n", m, n1);

   //second: n2 x k
   int n2 = int_rows;
   int k = int_cols;
   printf("n2: %d, k: %d\n", n2, k);
   
   if (n1 != n2){
      printf("column and row does not match between both matrices, can't calculate\n");
      return;
   }  
   int result_cols = 0;
   int largest_cols = 0;
   int largest_rows = 0;
   if (m > n2){
      largest_rows = m;
   } else {
      largest_rows = n2;
   }
   
   if (n1 > k){
      largest_cols = n1;
   } else {
      largest_cols = k;
   }
   
   if (rows > int_cols){//stored matrix is largest dim
      result_cols = int_cols;
   } else {//current matrix is largest dim
      result_cols = cols;
   }

   //new matrix m x k
   int *result_matrix = malloc(m* k * sizeof(int)); //flat buffer/single contiguous block of memory

 int fstMatrixRow[n1];
for (int i = 0; i < n1; i++){
	   fstMatrixRow[i] = 0;
  }
for (int i = 0 ; i < n1; i++){
     int currentValue = (fstMatrixRow)[i];

//printf("index: %d resultcurrentValue: %d\n", i,  currentValue);
}
 int offsetRow = 0;
 int counter = 0;

 int sndMatrixCol[k];
 int offsetCol = 0;

int* result = malloc(k); 
int offsetResultIndex = 0;
 for (int i = 0; i < m * n1; i=i+n1){
	 if (i == 0){
	    offsetRow = 0;
	 } else {
	    offsetRow += n1;
	 }
	 if (counter >= n1){
	    counter = 0;
	 }
//    printf("index i: %d\n", i);
    for (int j = offsetRow; j < offsetRow + n1; j++){
//printf("index j: %d, counter: %d\n", j, counter);
//printf("fstMatrixRow: %d, matrix_mem: %d\n", fstMatrixRow[counter], matrix_mem[j]);
       fstMatrixRow[counter] = matrix_mem[j];
//       printf("index j: %d, fstMatrixRow: %d, matrix_mem: %d\n", j, fstMatrixRow[counter], matrix_mem[j]);
       counter += 1;
    }
    result = iterateSumProductOverSndMatrixCol(fstMatrixRow, n1, matrix, k, n2, result);
for (int i = 0 ; i < k; i++){
     int currentValue = result[i];
//printf("RESULTTTTTindex: %d currentValue: %d\n", i,  currentValue);
}
      for (int i = offsetResultIndex, j = 0; i <  k + offsetResultIndex ;  i++, j++){ 
	   (result_matrix)[i] = result[j];
	   //printf("fill index: %d\n", i);
    }
      offsetResultIndex += k;
      
 }
for (int i = 0 ; i < k*m; i++){
     int currentValue = (*matrix)[i];
//printf("Mindex: %d currentValue: %d\n", i,  currentValue);
}
//printMatrixGivenDim(result_matrix, m, k);
//printMatrixGivenDim(*matrix, n2, k);

*matrix = realloc(*matrix, m * k *sizeof(int));
for (int i = 0; i < m * k; i++){
//   printf("i:%d\n", i);
   (*matrix)[i] = result_matrix[i];
}
//printMatrixGivenDim(*matrix, m, k);



//for (int i = 0 ; i < m; i++){
//     int currentValue = (fstMatrixRow)[i];
//printf("index: %d currentValue: %d\n", i,  currentValue);
//}
//printf("HELLO");
//for (int i = 0 ; i < k; i++){
//     int currentValue = (result)[i];
//
//printf("index: %d resultcurrentValue: %d\n", i,  currentValue);
//}
//
//for (int i = 0 ; i < k*m; i++){
//     int currentValue = (result_matrix)[i];
//printf("index: %d currentValue: %d\n", i,  currentValue);
//}

   int_rows = m; 
   int_cols = k;
   free(result);
   free(result_matrix);
}

void doActionInput(char** action, int rowSize, int colSize, int* matrix, int** matrixp){
   
   if (strcmp(action[0], "exit") == 0){
      free(matrix);
      free(action[0]);      
      delete_all_matrices();
      exit(EXIT_SUCCESS);
   } else if (strcmp(action[0], "set") == 0){
     //> set 2 2 7 //set value in row 2 column 2 to value 7
       setPointValue(atoi(action[3]), atoi(action[1]), atoi(action[2]), int_rows, int_cols, &matrix);
       free(action[0]); 
       free(action[1]);
       free(action[2]);
       free(action[3]);  
   } else if (strcmp(action[0], "fill") == 0){
     //> fill 3 //fill entire matrix with value 3 
      fillValues(atoi(action[1]), int_rows, int_cols, &matrix);
      free(action[0]); 
      free(action[1]);
   } else if (strcmp(action[0], "resize") == 0){
     //> resize 4 3 //resize existing matrix to 4 x 3
      resizeMatrix(atoi(action[1]), atoi(action[2]), int_rows, int_cols, matrixp);
      free(action[0]); 
      free(action[1]);
      free(action[2]);
   } else if (strcmp(action[0], "newrand") == 0){
     //> newrand 4 3 //make new 4 x 3 matrix with random values
     newRandomMatrix(atoi(action[1]), atoi(action[2]), int_rows, int_cols, matrixp);
     free(action[0]); 
     free(action[1]);
     free(action[2]);
   } else if (strcmp(action[0], "mult") == 0){
     //> mult 2 //multiply each value in matrix by 2
     multiplyMatrixWithInteger(atoi(action[1]), int_rows, int_cols, &matrix);
     free(action[0]); 
     free(action[1]);
   } else if (strcmp(action[0], "save") == 0){
     //> save 2 //save matrix with with key 2
     
     struct my_struct *thisMatrixToSave = malloc(sizeof(struct my_struct));
     thisMatrixToSave -> matrix_mem = malloc(int_rows * int_cols * sizeof(int));
     thisMatrixToSave -> id = atoi(action[1]);
     thisMatrixToSave -> column = int_cols;
     thisMatrixToSave -> row = int_rows;
    
     memcpy(thisMatrixToSave -> matrix_mem, matrix, int_rows * int_cols * sizeof(int));

     save_matrix(thisMatrixToSave);
     free(action[0]); 
     free(action[1]);

   } else if (strcmp(action[0], "load") == 0){
     //> load 2 //find the saved matrix by looking up key 2
     struct my_struct *thisMatrixToSave = find_matrix(atoi(action[1]));
     int *matrix_mem = thisMatrixToSave -> matrix_mem;
     int rows = thisMatrixToSave -> row;
     int cols = thisMatrixToSave -> column;
   
     if (int_rows * int_cols < rows * cols ){
        //works when loading big matrix to small current matrix
        *matrixp = realloc(*matrixp, rows * cols * sizeof(int));
	for (int i = 0; i < rows * cols;  i++){ 
	   (*matrixp)[i] = matrix_mem[i];
        }
     } else {
        memcpy(matrix, matrix_mem, rows * cols * sizeof(int)); 
     }

     //printMatrixGivenDim(matrix_mem, rows, cols);

     int_rows = rows;
     int_cols = cols;

     free(action[0]); 
     free(action[1]);

   } else if (strcmp(action[0], "mm") == 0){
      multiplyMatrixWithMatrix(atoi(action[1]), int_rows, int_cols, matrixp);
     free(action[0]); 
     free(action[1]);
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
   printf("> load 2 //find the saved matrix by looking up key 2\n");
   printf("> mm 2 //find the saved matrix by looking up key 2, then multiply it with matrix in current context\n");
   while(1){
    printMatrixGlobalDim(matrix);    

    char in[100];
    scanf(" %[^\n]", in);

    char** list = tokenize(in);

    parseProgramInput(list, int_rows, int_cols, matrix, &matrix);
        
    free(list);
    }
}
