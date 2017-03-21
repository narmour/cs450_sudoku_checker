#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void *checkRow(void *arg);
void *checkCol(void *arg);
void *checkBox(void *arg);

struct threadInfo{
    int task;
    int issue;
    int (*sudoku)[9];
};

void printSudoku(int (*s)[9]){
    for(int i =0; i < 9;i++){
        for(int j =0;j<9;j++){
            printf("%i ",s[i][j]);
            if((j+1) % 9 ==0)
                printf("\n");
            else if ((j+1) %3 ==0)
                printf("   ");
        }
        if((i+1) % 3 ==0)
            printf("\n");
    }
}

int main(int argc, char **argv){
    // read and store the input in a 2D array
    int sudoku[9][9];
    for(int i =0;i<9;i++)
        for(int j=0;j<9;j++)
            fscanf(stdin,"%i ",&sudoku[i][j]);

    printSudoku(sudoku);

    // for each row create a thread to determine if it contains digits 1 through 9
    pthread_t *rowThreads = malloc(9 * sizeof(pthread_t));
    struct threadInfo **rowStructs = malloc(9 * sizeof(struct threadInfo*));
    for(int i =0;i<9;i++){
        //printf("ival: %i\n",i);
        struct threadInfo *t = malloc(sizeof(struct threadInfo));
        t->task = i;
        t->issue =0;
        t->sudoku = sudoku;
        rowStructs[i] = t;
        pthread_create(&rowThreads[i],NULL,checkRow,(void*)t);

    }

    // for each col create a thread to determine if it contains digits 1 through 9
    pthread_t *colThreads = malloc(9 * sizeof(pthread_t));
    struct threadInfo **colStructs = malloc(9 * sizeof(struct threadInfo*));
    for(int i =0;i<9;i++){
        struct threadInfo *t = malloc(sizeof(struct threadInfo));
        t->task = i;
        t->issue =0;
        t->sudoku = sudoku;
        colStructs[i] = t;
        pthread_create(&colThreads[i],NULL,checkCol,(void*)t);
    }

    // for each 3x3 box create a thread to determine if it contains digits 1 through 9
    pthread_t *boxThreads = malloc(9 * sizeof(pthread_t));
    struct threadInfo **boxStructs = malloc(9 * sizeof(struct threadInfo*));
    for(int i =0;i<9;i++){
        struct threadInfo *t = malloc(sizeof(struct threadInfo));
        t->task = i;
        t->issue =0;
        t->sudoku = sudoku;
        boxStructs[i] = t;
        pthread_create(&boxThreads[i],NULL,checkBox,(void*)t);
    }

    // wait for all threads to complete
    for(int i =0;i<9;i++){
        pthread_join(rowThreads[i],NULL);
        pthread_join(colThreads[i],NULL);
        pthread_join(boxThreads[i],NULL);
    }

    // check if any threads found errors
    int errors = 0;// 0 if no errors, set to 1 if i find an error
    for(int i =0;i <9;i++){
        if(rowStructs[i]->issue == 1){
            printf("Row %i doesn't have the required values.\n",rowStructs[i]->task);
            errors=1;
        }
        if(colStructs[i]->issue == 1){
            printf("Column %i doesn't have the required values.\n",colStructs[i]->task);
            errors=1;
        }
        if(boxStructs[i]->issue == 1){
            printf("Subgrid %i doesn't have the required values.\n",boxStructs[i]->task);
            errors=1;
        }

        // free memory
        free(rowStructs[i]);
        free(colStructs[i]);
        free(boxStructs[i]);
    }

    // free some more stuff
    free(rowStructs);
    free(colStructs);
    free(boxStructs);

    if(!errors)
        puts("The input is a valid Sudoku.");
    else
        puts("The input is not a valid Sudoku.");

    return 0;
}

void *checkRow(void *arg){
    struct threadInfo *t = (struct threadInfo*)arg;

    
    int row = t->task;
    for(int i =0;i<9;i++){
        int c = t->sudoku[row][i];
        for(int j =0;j<9;j++){
            if(i!=j && t->sudoku[row][j] == c){
                t->issue = 1;
            }
        }
    }
}

void *checkCol(void *arg){
    struct threadInfo *t = (struct threadInfo*)arg;
    int col = t->task;
    for(int i =0;i<9;i++){
        int c = t->sudoku[i][col];
        for(int j =0;j<9;j++){
            if(i!=j && t->sudoku[j][col] == c){
                t->issue = 1;
            }
        }
    }

}
void *checkBox(void *arg){
    struct threadInfo *t = (struct threadInfo*)arg;
    int boxRowStart =(t->task/3)*3;  
    int boxColStart =(t->task % 3) * 3;

    for(int i=boxRowStart;i<boxRowStart+3;i++){ 
        for(int j = boxColStart;j<boxColStart+3;j++){
            int c = t->sudoku[i][j];
            for(int k=boxRowStart;k<boxRowStart+3;k++){
                for(int l = boxColStart;l<boxColStart+3;l++){
                    if(k!=i && l!=j && c == t->sudoku[k][l]){
                        t->issue = 1;
                    }
                }
            }
        }
    }
}
