#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAXSIZE 10
#define MAXCAGES 20

/**
 * Struct to represent the cells of the kenken, example: 0a, 4c
 */
typedef struct Cell {

    char group;
    int num;

} Cell;

/**
 * Struct to represent the points of the cages, example: (3,4)
 */
typedef struct Point {

    int row;
    int col;

} Point;

/**
 * Struct to represent the target, examples: 16*, 4+
 */
typedef struct Target {

    int number;
    char operator;

} Target;

/**
 * Struct to represent a single cage, example: {(3,4), (4,4)}, listSize = 2
 */
typedef struct Cage {

    Point points[MAXCAGES];
    int listSize;

} Cage;

// GLOBAL VARIABLES
Cell kenken[MAXSIZE][MAXSIZE];
Target targets[MAXCAGES];
Cage groups[MAXCAGES];
unsigned int cages = 0;
unsigned int size = 0;

/**
 * Prints the current state of kenken to the screen.
 */
void printKenken() {
    char buff[1024] = "";
    int i, j, x;

    for (i = 0; i < size; i++) {
        strcat(buff, "+");
        for (x = 0; x < size; x++)
            strcat(buff, "------+");
        strcat(buff, "\n|");
        for (j = 0; j < size; j++) {
            Cell current = kenken[i][j];
            char buffer[50] = "";
            if (current.num > 0) {
                snprintf(buffer, 50, "%3d", current.num);
            } else {
                snprintf(buffer, 50, "%3s", "");
            }
            buffer[strlen(buffer)] = current.group;
            strcat(buff, buffer);
            strcat(buff, "  |");
        }
        strcat(buff, "\n");
    }
    strcat(buff, "+");
    for (x = 0; x < size; x++)
        strcat(buff, "------+");
    strcat(buff, "\n\n\0");

    printf("%s", buff);
}

/**
 * Writes the current state of kenken to an outputFile
 * @param outputFile
 */
void writePuzzle(char *outputFile) {
    char buff[1024] = "";
    int i, j, x;

    for (i = 0; i < size; i++) {
        strcat(buff, "+");
        for (x = 0; x < size; x++)
            strcat(buff, "------+");
        strcat(buff, "\n|");
        for (j = 0; j < size; j++) {
            Cell current = kenken[i][j];
            char buffer[50] = "";
            if (current.num > 0) {
                snprintf(buffer, 50, "%3d", current.num);
            } else {
                snprintf(buffer, 50, "%3s", "");
            }
            buffer[strlen(buffer)] = current.group;
            strcat(buff, buffer);
            strcat(buff, "  |");
        }
        strcat(buff, "\n");
    }
    strcat(buff, "+");
    for (x = 0; x < size; x++)
        strcat(buff, "------+");
    strcat(buff, "\n\n\0");

    FILE *file = fopen(outputFile, "w");
    fprintf(file, buff);
    fclose(file);
}

/**
 * Prints the target of the cages to the screen.
 */
void printTargets() {
    int i;
    for (i = 0; i < cages - 1; i++) {
        printf("%c=%d%c ", 'a' + i, targets[i].number, targets[i].operator);
    }
    printf("%c=%d%c\n", 'a' + i, targets[i].number, targets[i].operator);
}

/**
 * Third rule of kenken, the cages' sum or product need to the same as requested value when
 * all points of the group have a number, else we can return true if the current sum or product of the
 * points strictly did not exceed the target value when the operator is + and atleast not exceeded when
 * the operator is * in case the rest of the points will be 1 so the product wont get bigger.
 *
 * @param cage, the cage we are testing
 * @returns true if current cage is "valid" based on its target value
 */
bool groupCheck(int cage, bool *exit) {
    char operator = targets[cage].operator;
    bool filled = true;

    int i = 0;
    if (operator == '+') {
        int accumulator = 0;
        for (i = 0; i < groups[cage].listSize; i++) {
            int c = kenken[groups[cage].points[i].row][groups[cage].points[i].col].num;
            if (c != 0) {
                accumulator += c;
            } else {
                filled = false;
                if(accumulator >= targets[cage].number)
                    *exit = true;
                break;
            }
        }
        return (accumulator == targets[cage].number && filled) || (accumulator < targets[cage].number && !filled);
    } else {
        int accumulator = 1;
        for (i = 0; i < groups[cage].listSize; i++) {
            int c = kenken[groups[cage].points[i].row][groups[cage].points[i].col].num;
            if (c != 0) {
                accumulator *= c;
            } else {
                filled = false;
                if(accumulator >= targets[cage].number)
                    *exit = true;
                break;
            }
        }
        return (accumulator == targets[cage].number && filled) || (accumulator <= targets[cage].number && !filled);
    }
}

/**
 * Second rule of kenken, no duplicated nums in columns.
 *
 * @param row we are and want to check indices above us for a duplicated num
 * @param col to be checked
 * @param num
 * @returns if this num can be placed at this col
 */
bool colCheck(int row, int col, int num) {
    for (; row >= 0; row--) {
        if (kenken[row][col].num == num)
            return false;
    }
    return true;
}

/**
 * First rule of kenken, no duplicated nums in rows.
 *
 * @param row, to be checked
 * @param col, col we are and want to check indices before us for a duplicated num
 * @param num
 * @returns if this num can be placed at this row
 */
bool rowCheck(int row, int col, int num) {
    for (; col >= 0; col--) {
        if (kenken[row][col].num == num)
            return false;
    }
    return true;
}

/**
 * @brief Recursively fills the first non-empty cell according to the rules of kenken,
 *        with the smallest possible num.
 *
 * @version 1.1 Converted to iterative version leading to a space complexity of 0(1)
 *
 * If the next instance of solve() cannot place any value on its responsible cell
 * it will return false to its previous instance and the previous will try a higher
 * value.With this algorithm in mind if the row passed to the function becomes
 * equal to size(out of bounds) means all previous cells are filled with a value.
 * Otherwise if the very first method returns false means that the first non-empty
 * cell tried all values so the puzzle has no possible solutions.
 *
 * @param row, the row of the cell solve() will try to put value
 * @param col, the col of the cell solve() will try to put value
 * @returns true if puzzle is solved, false if could not place a value at kenken[i][j]
 */
bool solve(int row, int col) {
    int num = 0;
    start:
    while (row < size && row >= 0) {
        bool exit = false;

        for (num = kenken[row][col].num + 1; num <= size; num++) {
            if (rowCheck(row, col, num) && colCheck(row, col, num)) {// ADVANCE
                kenken[row][col].num = num;

                if (groupCheck(kenken[row][col].group - 'a', &exit)) {
                    col += 1;
                    if (col == size) {
                        row += 1;
                        col = 0;
                    }
                    goto start;
                }
                if(exit)
                    break;
            }
        }
        // BACKTRACK
        kenken[row][col].num = 0;

        col -= 1;
        if (col < 0) {
            row -= 1;
            col = size - 1;
        }
    }
    return row == size;
}

/**
 *
 * @param i, the requested row for the value to be inserted at.
 * @param j, the requested col for the value to be inserted at.
 * @param val
 * @param filed, will increment filled upon succesful insertion / decrement on deletion.
 */
void insertValue(int i, int j, int val, int *filed) {
    if (val == 0 && kenken[i][j].num != 0) {
        kenken[i][j].num = 0;
        *filed = *filed - 1;
        printf("\nValue cleared!\n\n");
    } else if (val == 0 && kenken[i][j].num == 0) {
        printf("\nValue already cleared!\n\n");
    } else if (kenken[i][j].num != 0) {
        printf("\nThere is already a value at (%d, %d)\n\n", i + 1, j + 1);
    } else {
        if (rowCheck(i, j, val) && colCheck(i, j, val)) {
            kenken[i][j].num = val;
            bool exit = false;
            if (groupCheck(kenken[i][j].group - 'a', &exit)) {
                *filed = *filed + 1;
                printf("\nValue inserted!\n\n");
            } else {
                kenken[i][j].num = 0;
                printf("\nInvalid move.\n\n");
            }
        } else {
            printf("\nInvalid move.\n\n");
        }
    }
}

/**
 * Shows a menu for the format of the commands that user can execute.
 */
void showMenu() {
    printf("Enter your command in the following format:\n"
           ">i,j=val: for entering val at position (i,j)\n"
           ">i,j=0 : for clearing cell (i,j)\n"
           ">0,0=0 : for saving and ending the game\n"
           "Notice: i,j,val numbering is from ");
    printf("[%d..%d]\n>", 1, size);
}

/**
 * Interface function for the user to manually solve the puzzle.
 */
void play(char *inputfile) {
    int i, j, val, filled = 0;
    do {
        printTargets();
        printKenken();
        showMenu();

        if (scanf("%d,%d=%d", &i, &j, &val) != 3) {
            while (getchar() != '\n') {};
            printf("\nWrong format of command.\n\n");
            continue;
        }

        if (i < 0 || j < 0 || val < 0 || i > size || j > size || val > size) {
            printf("\nIllegal value insertion!\n\n");
            continue;
        }

        if (i > 0 && j > 0) {
            insertValue(i - 1, j - 1, val, &filled);

            if (filled == size * size) {
                printf("***Game solved***\n");
                printKenken();
                return;
            }
        } else if (i == 0 && j == 0 && val == 0) {
            char out[100] = "out-";
            strcat(out, inputfile);
            printf("\nGame saved to %s\n\n", out);
            writePuzzle(out);
            return;
        } else {
            printf("\nWrong format of command.\n\n");
        }
    } while (i > 0 || j > 0 || val > 0);
}

/**
 * This method initialises our 3 data structures: a 2D array of Cell struct which represents the puzzle,
 * an array of type Target(of the cages) and an array of type Cage with their points.
 *
 * @param fileName : Will try to open this file containing information about kenken
 * @returns a boolean to check if file was read properly or info was correct
 */
bool readPuzzle(char *fileName) {
    FILE *file = fopen(fileName, "r");

    if (file == NULL) {
        printf("\nFileError: Could not find such file in directory.\n\n");
        return false;
    }

    if (fscanf(file, "%d", &size) != 1) {
        printf("\nError at first line: Expected a single num for the size: 2 < num <= %d\n\n", MAXSIZE);
        return false;
    }

    if (size < 2 || size > MAXSIZE) {
        printf("\nSize needs to be bigger than 2 and less than %d, your input was: %d\n\n", MAXSIZE, size);
        return false;
    }

    int cells_filled = 0;
    char groupChar = 'a';
    int target = 0;
    char operator = '\0';

    // WHILE LOOP FETCHES THE NUMBER AND OPERATOR
    while (fscanf(file, "%d%c", &target, &operator) == 2) {
        if (operator != '+' && operator != '*') {
            printf("\nOperators need to be either + or *, your input at line %d was: %c\n\n", cages + 2, operator);
            return false;
        }

        // WHILE LOOP FETCHES ALL THE POINTS OF THE LINE AFTER THE NUMBER AND OPERATOR
        int row = -1, col = -1;
        while (fscanf(file, " (%d,%d)", &row, &col) == 2) {
            if (row < 1 || row > size || col < 1 || col > size) {
                printf("\nOutOfBounds point at line %d: (%d,%d)\n\n", cages + 2, row, col);
                return false;
            }

            // INITIALISING THE GROUPS
            groups[cages].points[groups[cages].listSize].row = row - 1;
            groups[cages].points[groups[cages].listSize].col = col - 1;

            if (kenken[row - 1][col - 1].group != '\0') {
                printf("\nDuplicated point trying to change values of previous point at line %d : (%d,%d)\n\n",
                       cages + 2, row, col);
                return false;
            }
            kenken[row - 1][col - 1].group = groupChar;
            kenken[row - 1][col - 1].num = 0;
            groups[cages].listSize += 1;
            cells_filled += 1;
        }
        if (row == -1 || col == -1) {
            printf("\nLine %d did not contain any valid points.\n\n", cages + 2);
            return false;
        }

        // INITIALISING THE TARGETS OF EACH GROUP
        targets[cages].number = target;
        targets[cages].operator = operator;
        cages += 1;
        groupChar += 1;

        if (cages > 26) {
            printf("\nNumber of cages needs to be less than or equals to 26(letters of alphabet)\n\n");
            return false;
        }
    }

    fclose(file);

    if (cages < 1) {
        printf("\nFile did not contain any cages or something is wrong with the file format.\n\n");
        return false;
    }

    if (cells_filled != size * size) {
        printf("\nNot all points of the kenken were initialised, check your file!\n\n");
        return false;
    }
    return true;
}

int main(int args, char *argv[]) {
    // CHECK IF CORRECT NUMBER OF ARGS
    if (args != 3) {
        printf("\nArgumentsError! Incorrect number of arguments given, Expected: 3.\nCorrect format example: ./kenken -i inputFileName.txt\n\n");
        return 1;
    }

    if (!readPuzzle(argv[2])) {
        return 1;
    }

    if (strcmp(argv[1], "-i") == 0) {
        play(argv[2]);
    } else if (strcmp(argv[1], "-s") == 0) {
        if (solve(0, 0)) {
            printf("\n***AUTOMATICALLY SOLVED***\n");
            printKenken();
        } else {
            printf("\nThere is no solution for this kenken...\n");
        }
    } else {
        printf("\nModes should be -i or -s.\n");
    }
    return 0;
}
