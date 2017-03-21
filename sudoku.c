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
            printf("\n\n");
    }





}


int main(void){
    // read and store the input in a 2D array
    FILE *input = fopen("input.txt","r");
    if(!input){
        puts("error opening input.txt");
        exit(1);
    }
    int sudoku[9][9];
    for(int i =0;i<9;i++)
        for(int j=0;j<9;j++)
            fscanf(input,"%i ",&sudoku[i][j]);

    printSudoku(sudoku);

    // for each row create a thread to determine if it contains digits 1 through 9
    pthread_t *rowThreads = malloc(9 * sizeof(pthread_t));
    for(int i =0;i<9;++i){
        //printf("ival: %i\n",i);
        struct threadInfo *t = malloc(sizeof(struct threadInfo));
        t->task = i;
        t->issue =0;
        t->sudoku = sudoku;
        pthread_create(&rowThreads[i],NULL,checkRow,(void*)t);

    }

    // for each col create a thread to determine if it contains digits 1 through 9
    pthread_t *colThreads = malloc(9 * sizeof(pthread_t));
    for(int i =0;i<9;i++){
        struct threadInfo *t = malloc(sizeof(struct threadInfo));
        t->task = i;
        t->issue =0;
        t->sudoku = sudoku;
        pthread_create(&colThreads[i],NULL,checkCol,(void*)t);
    }

    // for each 3x3 box create a thread to determine if it contains digits 1 through 9
    pthread_t *boxThreads = malloc(9 * sizeof(pthread_t));
    for(int i =0;i<9;i++){
        struct threadInfo *t = malloc(sizeof(struct threadInfo));
        t->task = i;
        t->issue =0;
        t->sudoku = sudoku;
        pthread_create(&boxThreads[i],NULL,checkBox,(void*)t);
    }

    for(int i =0;i<9;i++){
        pthread_join(rowThreads[i],NULL);

    }






    return 0;
}

void *checkRow(void *arg){
    struct threadInfo *t = (struct threadInfo*)arg;

    
    int row = t->task;
    printf("task %i\n",row);
    for(int i =0;i<9;i++){
        int c = t->sudoku[row][i];
        for(int j =0;j<9;j++){
            if(i!=j && t->sudoku[row][j] == c){
                printf("found error in row: %i\n",t->task);
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
                printf("found error in col: %i\n",t->task);
                t->issue = 1;
            }
        }
    }

}
void *checkBox(void *arg){
    struct threadInfo *t = (struct threadInfo*)arg;
    int boxRow =(t->task/3)*3;  
    int boxCol =(t->task);
    printf("task: %i     div: %i\n",t->task,t->task/3);


}
