#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
int int_rows;
int int_cols;

void printMatrix(int *matrix){
    for(int i = 0; i < int_rows; i++){ 
       for(int j = 0; j < int_cols; j++){
          printf("%d\t", matrix[i * int_cols + j]);
        //printf("%d\t", *(matrix + i * cols + j));
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

void doActionInput(char** action, int rowSize, int colSize, int** matrix){
   //TODO pattern match based on keyword set, fill, new, newrand

   if (strcmp(action[0], "set") == 0){
       //> set 2 2 7 //set value in row 2 column 2 to value 7
       setPointValue(atoi(action[3]), atoi(action[1]), atoi(action[2]), int_rows, int_cols, matrix);
   } else if (strcmp(action[0], "fill") == 0){
      //> fill 3 //fill entire matrix with value 3 
      fillValues(atoi(action[1]), int_rows, int_cols, matrix); 
   } else if (strcmp(action[0], "resize") == 0){
     //> resize 4 3 //make new 4 x 3 matrix
      resizeMatrix(atoi(action[1]), atoi(action[2]), int_rows, int_cols, matrix);
   } 
     //> newrand 4 3 //make new 4 x 3 matrix with random values
   else {
      printf("Unrecognized function\n");
   }

}

void parseProgramInput(char** in, int rowSize, int colSize, int** matrix){
  //TODO validate arg 'in' along with dimensions rowSize and colSize

  if (validateInput(in) == false){
     printf("Error incorrect input or dimension \n");
     return;
  }
  doActionInput(in, int_rows, int_cols, matrix);   
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
   while(1){
    printMatrix(matrix);    

    char in[100];
    scanf(" %[^\n]", in);
    //printf("Scanned: %s", in);

    char** list = tokenize(in);

    parseProgramInput(list, int_rows, int_cols, &matrix);
    free(list);
    }
}
