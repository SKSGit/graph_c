An interactive matrix calculator. Create, resize, save, load, and perform arithmetic on matrices from a command-line REPL.

Requires [uthash.h](https://troydhanson.github.io/uthash/) — download and place it in the project directory.

## Build

```
make
```

Or manually:

```
gcc -Wall -Wextra -std=c11 -O2 -o main main.c -lm
```

## Run

```
./main <rows> <cols>
```

## Commands

| Command | Description |
|---|---|
| `set <r> <c> <v>` | Set element at row r, column c to value v |
| `fill <v>` | Fill entire matrix with value v |
| `resize <r> <c>` | Resize matrix to r×c (new cells initialised to 0) |
| `newrand <r> <c>` | Replace matrix with a new r×c random matrix |
| `mult <v>` | Multiply every element by scalar v |
| `save <id>` | Save current matrix under integer id |
| `load <id>` | Load saved matrix by id |
| `mm <id>` | Multiply saved[id] × current matrix |
| `tr` | Transpose current matrix |
| `id` | Overwrite current (square) matrix with identity |
| `ma <id>` | Add saved matrix to current (dimensions must match) |
| `ms <id>` | Subtract saved matrix from current (dimensions must match) |
| `eq` | Print current matrix as a system of linear equations |
| `exit` | Quit |

## Example

```
./main 7 3 < <(printf "fill 2\nset 1 2 9\nsave 2\nnewrand 3 5\nfill 4\nmm 2\nexit")
```

1. Creates a 7×3 matrix filled with 2s
2. Sets row 1, column 2 to 9
3. Saves it as id 2
4. Generates a new random 3×5 matrix
5. Fills it with 4s
6. Multiplies saved[2] (7×3) by current (3×5) → result is 7×5

## Tests

```
make test
```

![demo](https://github.com/user-attachments/assets/92ba7121-681b-45cc-b7a8-b6b747eb460b)
