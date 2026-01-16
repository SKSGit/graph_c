#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
int int_rows;
int int_cols;
void printMatrix(int *matrix,int rows, int cols){
    for(int i = 0; i < rows; i++){ 
       for(int j = 0; j < cols; j++){
          printf("%d\t", *(matrix + i * cols + j));
       }
       printf("\n"); 
   }
}

void setPointValue(int value, int row, int col, int rowSize, int colSize, int matrix[rowSize][colSize]){
  matrix[row][col] = value; //must pass dimensions rowSize colSize, otherwise C cannot determine where to place value as the array would then be varriable length array (VLA). So pointer arithmetic would fail
}

void fillValues(int value, int rows, int cols, int matrix[rows][cols]){
    for(int i = 0; i < rows; i++){ 
       for(int j = 0; j < cols; j++){
          matrix[i][j] = value;
       }
   }
}

bool validateInput(char input){
  return true;//TODO (0 < input || input  < int_rows -1) && (0 < input || input < int_cols-1);
}

void parseProgramInput(char** in[], int rowSize, int colSize, int matrix[rowSize][colSize]){
//TODO validate arg 'in' along with dimensions rowSize and colSize

  if (validateInput(in) == false){
     printf("Error incorrect input or dimension \n");
     return;
  }
  //TODO tokenize(in);
  doActionInput(in,int_rows, int_cols, matrix);   
  //TODO doActionInput(in); //parse input keywords; set,fill,new,newrand 
  //> set 2 2 7 //set value in row 2 column 2 to value 7
  //> fill 3 //fill entire matrix with value 3 
  //> new 4 3 //make new 4 x 3 matrix
  //> newrand 4 3 //make new 4 x 3 matrix with random values
}

void doActionInput(char** action[], int rowSize, int colSize, int matrix[rowSize][colSize]){
 //TODO pattern match based on keyword set, fill, new, newrand


   //printf("ACTION first %s\n", action[0]);

   if (strcmp(action[0], "set") == 0){
       //printf("ACTION INNER SET %s %s %s\n",action[1],action[2], action[3]);
       setPointValue(atoi(action[3]), atoi(action[1]), atoi(action[2]), int_rows, int_cols, matrix);
   } else if (strcmp(action[0], "fill") == 0){
      fillValues(atoi(action[1]), int_rows, int_cols, matrix); 
   } 

   else {
      printf("Unrecognized function");
   }

}

int main(int argc, char *argv[]){

   int_rows = *argv[1] - '0'; //first matrix dimension program arg
   int_cols = *argv[2] - '0'; //second matrix dimension program arg

   int matrix[int_rows][int_cols] = {}; //default fills matrix with zeros

   printf("This is graph program: Hello %s \n","User");
   printf("The Graph dimension is %i x %i\n", int_rows, int_cols);
   printf("> set 2 2 7 //set value in row 2 column 2 to value 7\n");
   printf("> fill 3 //fill entire matrix with value 3\n"); 
   while(1){
    printMatrix((int *)matrix, int_rows, int_cols);    

    char in[100];
    scanf(" %[^\n]", in);
    //printf("Scanned: %s", in);

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


     parseProgramInput(list,int_rows, int_cols, matrix);
    
    free(temp);
       
     for (int i = 0; i < count; i++){

        //printf("Token: %s\n", list[i]);
        free(list[i]);
     }
    free(list);
     }
}

