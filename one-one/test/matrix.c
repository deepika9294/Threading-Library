/*
* Program to perform 2d matrix multiplication by creating 3 threads.
*/


#include <stdio.h>
#include "../dthread.h"
#include <stdlib.h>

// structure for matrix
typedef struct {
    int **mat;
    int r;
    int c;
}matrix;

typedef struct {
    int row_start;
    int row_end;
}rows;

// Matrix declaration
matrix a,b,res;
int final_check = 1;


// initialisation of matrix
void init(matrix *t, int c) {
    t->mat = (int **)malloc(sizeof(int *) * t->r);
    for(int i = 0; i < t->r; i++ ) {
        t->mat[i] = (int *)malloc(sizeof(int) * t->c); 
    }
    // initialising the matrix with c
    for(int i = 0; i < t->r; i++) {
        for(int j = 0; j < t->c; j++) {
            t->mat[i][j] = c;
        }
    }
}

// reading the values into 2d array
void read_input(matrix *t, FILE *file) {

    for(int i = 0; i < t->r; i++) {
        for(int j = 0; j < t->c; j++) {
            fscanf(file, "%d", &(t->mat[i][j]));
        }
    }
}

void print_matrix(matrix *t) {
    printf("\n");
    // t->mat[5][5] = 34;
    for(int i = 0; i < t->r; i++) {
        for(int j = 0; j < t->c; j++) {
            // printf("%d ", t->mat[i][j]);
            if(t->mat[i][j] != 80){
                final_check = 0;
            }
        }
        // printf("\n");
    }
    if(final_check == 1) {
        printf("**PASSED** : MATRIX TEST with rows=%d, columns=%d\n", t->r, t->c);
    }
    else {
        printf("**FAILED** : MATRIX TEST with rows=%d, columns=%d\n", t->r, t->c);

    }

}

// freeing the space
void destroy_matrix(matrix *t) {
    for(int i = 0; i<t->r; i++) {
        free(t->mat[i]);
    }
    free(t->mat);
}


void* partial_multiply(void *arg) {
    int sum;
    rows *temp = (rows *)arg; 
    int start = temp->row_start;
    int end = temp->row_end + 1;
    // printf("D %d %d \n", start, end);
    for(int i = start; i < end; i++) {
        for(int j = 0; j < b.c; j++) {
            sum = 0;
            for( int k = 0; k < a.c; k++) {
                sum += a.mat[i][k] * b.mat[k][j];

            }
            res.mat[i][j] = sum;
        }
    }
}



int main() {
    dthread_t tid[3];
    rows args[3];
    int temp;
    FILE* file; 

    file = fopen("./test/mat_data.txt", "r");
    if(file == NULL) {
        printf("no such file");
        exit(EXIT_FAILURE);
    }
    fscanf(file,"%d %d", &(a.r), &(a.c));
    init(&a,0);
    read_input(&a, file);

    fscanf(file,"%d %d", &(b.r), &(b.c));
    init(&b,0);
    read_input(&b,file);
    dthread_init();
    fclose(file);

    // initialising size of output matrix if they are multiplicable
    if(a.c == b.r) {
        res.r = a.r;
        res.c = b.c;
    }
    else {
        printf("Matrix cannot be multiplied");
        return 0;
    }
    init(&res,0);
    
    // dividing the array in 3 parts by rows
    args[0].row_start = 0;
    args[0].row_end = a.r / 3 - 1;
    args[1].row_start = args[0].row_end + 1;
    args[1].row_end = args[1].row_start + args[0].row_end ;
    args[2].row_start = args[1].row_end + 1;
    args[2].row_end = a.r - 1;

    // debugging 
    // for(int i = 0; i < 3; i++)
    //     printf("%d %d \n", args[i].row_start, args[i].row_end);
 
    // creating three different threads
    for(int i = 0; i < 3; i++) {
        dthread_create(&tid[i], partial_multiply,&args[i]);
    }

    for(int i = 0; i < 3; i++) {
        dthread_join(tid[i], NULL);
    }    


    print_matrix(&res);

    destroy_matrix(&a);
    destroy_matrix(&b);
    destroy_matrix(&res);

    return 0;

}