This project uses [uthash.h](https://troydhanson.github.io/uthash/), add this file to the directory in order to compile successfully.

Steps to run:
1. Add utash.h to directory
2. compile ```gcc main.c```
3. run ```./a.out 3 3```

Example:
```printf "fill 2\n set 1 2 9\nsave 2\nnewrand 3 5\nfill 4\nmm 2\nexit") | ./a.out 7 3```

The above command, executes the following inside the program:
1. generates 7x3 matrix
2. fills above 7x3 matrix with value 2s
3. sets value of row 1 column 2 to value 9
4. save the above 7x3 matrix with key 2
5. generates newrandom 3x5 matrix
6. fills 5x3 matrix with 4s
7. matrix multiply previously saved 7x3 matrix with current 5x3 matrix
