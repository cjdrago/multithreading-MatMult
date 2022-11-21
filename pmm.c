// C Program to multiply two matrix using pthreads without
// use of global variables
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <math.h>

int MAX_LINE_LENGTH = 1000*1000;

int ***divMat(int row, int col, int **mat);
void printMat(int row, int col, int **mat);
int **splitMult(int indexA, int indexB, int ***splitA, int ***splitB);
int **matPlus(int **matX, int **matY);
int **checkEvenMat(int **mat, int n, int m);
int **allocateMat(int **mat, int rows, int cols);
int checkLineMax(int num1, int num2);


struct arg_struct {
    int ***A;
    int ***B;
    int ***C;
    int n;
    int p;
    int m;
}args;


// Each thread computes single element in the resultant matrix
void *sumMult(void *args)
{
    
    struct arg_struct *data = args;

    for(int c=0; c<data->m*data->n/4; c++) {
        data->C[c][0][0] = 0;
        data->C[c][0][1] = 0;
        data->C[c][1][0] = 0;
        data->C[c][1][1] = 0;

        int j = c%(data->m/2);
        int i = (c - j)/(data->m/2);
        for(int k=0; k < data->p/2; k++) {
            data->C[c] = matPlus(data->C[c],
                        splitMult(i*data->p/2+k, k*data->m/2+j, 
                        data->A, data->B));
        }
    };
    pthread_exit(data->C);
};


int main(int argc, char **argv)
{
    char input[10];

    scanf("%s", input);
    int n = atoi(input);
    scanf("%s", input);
    int p = atoi(input);
    scanf("%s", input);
    int m = atoi(input);
    
    int **matA, **matB;
    matA = allocateMat(matA, n, p);
    matB = allocateMat(matB, p, m);

    for(int i = 0; i<n; i++)
        for(int j = 0; j<p; j++){
            scanf("%s", input);
            matA[i][j] = atoi(input);
        }

    for(int i = 0; i<p; i++)
        for(int j = 0; j<m; j++){
            scanf("%s", input);
            matB[i][j] = atoi(input);
        }
               

    int **evenA = checkEvenMat(matA, n, p);
    int **evenB = checkEvenMat(matB, p, m);

    int orgN = n;
    int orgM = m;
    
    n = n+n%2;
    p = p+p%2;
    m = m+m%2;


    int ***splitA = divMat(n, p, evenA);
    int ***splitB = divMat(p, m, evenB);

    int i, j, k;
    int max = (n * m)/4;

    pthread_t *threads;
    threads = (pthread_t *)malloc(max * sizeof(pthread_t));

    int count = 0;
    int ***splitC = NULL;


    splitC= (int***)malloc(max* sizeof(int**));
 
    if (splitC == NULL)
    {
        fprintf(stderr, "Out of memory");
        exit(0);
    }
 
    for (int i = 0; i < (max); i++)
    {
        splitC[i] = (int**)malloc(2 * sizeof(int*));
 
        if (splitC[i] == NULL)
        {
            fprintf(stderr, "Out of memory");
            exit(0);
        }
        for (int j = 0; j < 2; j++)
        {
            splitC[i][j] = (int*)malloc(2 * sizeof(int));
            if (splitC[i][j] == NULL)
            {
                fprintf(stderr, "Out of memory");
                exit(0);
            }
        }
    };

    struct arg_struct args;
    args.A = splitA;
    args.B = splitB;
    args.C = splitC;
    args.n = n;
    args.p = p;
    args.m = m;

    // creating threads
    for(int i = 0; i < max; i++)
        pthread_create(&threads[i], NULL, sumMult,  (void *)&args);
    for(int i = 0; i < max; i++)
        pthread_join(threads[i], NULL);

    int **retMatC;

    retMatC = malloc(sizeof(int *)*n);
    for(int row = 0; row<n; row++)
        retMatC[row] = malloc(sizeof(int)*m);

    k = 0;
    for (int i = 0; i < n; i = i + 2){
        for (int j = 0; j < m; j = j + 2){
            for (int x = 0; x < 2; x++)
                for (int y = 0; y < 2; y++)
                {

                    retMatC[i+x][j+y] = args.C[k][x][y];
                }
            k++;
        }
    };

    int **retMatCFinal;

    retMatCFinal = malloc(sizeof(int *)*orgN);
    for(int row = 0; row<orgN; row++)
        retMatCFinal[row] = malloc(sizeof(int)*orgM);

    for (int i = 0; i < orgN; i++)
        for (int j = 0; j < orgM; j++)
            retMatCFinal[i][j] = retMatC[i][j];


    printMat(orgN,orgM, retMatCFinal);

    return 0;
};


int **checkEvenMat(int **mat, int n, int m){

    int row = n + n%2;
    int col = m + m%2;
    int **evenMat;
    evenMat = malloc(sizeof(int *)*row);
    for(int i =0; i<row; i++)
        evenMat[i] = malloc(sizeof(int)*col);

    for(int i = 0; i<row; i++)
        for(int j =0; j<col; j++){
            if(i >= n || j >= m) evenMat[i][j] = 0;
            else evenMat[i][j] = mat[i][j];

        };


    return evenMat;
}


int **splitMult(int indexA, int indexB, int ***splitA, int ***splitB){
    int **retMat;
    retMat = malloc(sizeof(int*)*2);

    for(int i =0; i<2; i++){
        retMat[i] = malloc(sizeof(int)*2);
    };

    retMat[0][0] = splitA[indexA][0][0]*splitB[indexB][0][0] + splitA[indexA][0][1]*splitB[indexB][1][0];

    retMat[0][1] = splitA[indexA][0][0]*splitB[indexB][0][1] + splitA[indexA][0][1]*splitB[indexB][1][1];

    retMat[1][0] = splitA[indexA][1][0]*splitB[indexB][0][0] + splitA[indexA][1][1]*splitB[indexB][1][0];

    retMat[1][1] = splitA[indexA][1][0]*splitB[indexB][0][1] + splitA[indexA][1][1]*splitB[indexB][1][1];

    return retMat;

}

int **matPlus(int **matX, int **matY) {
    int **retMat;
    retMat = malloc(sizeof(int*)*2);

    for(int i =0; i<2; i++){
        retMat[i] = malloc(sizeof(int)*2);
    };

    retMat[0][0] = matX[0][0] + matY[0][0];
    retMat[0][1] = matX[0][1] + matY[0][1];
    retMat[1][0] = matX[1][0] + matY[1][0];
    retMat[1][1] = matX[1][1] + matY[1][1];

    return retMat;

}



void printMat(int row, int col, int **mat)
{
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
            printf("%d ", mat[i][j]);
        printf("\n");
    };
    printf("\n");
}

int ***divMat(int row, int col, int **mat)
{
    int*** retMat = (int***)malloc( ((row * col) / 4)* sizeof(int**));
 
    if (retMat == NULL)
    {
        fprintf(stderr, "Out of memory");
        exit(0);
    }
 
    for (int i = 0; i < ((row * col) / 4); i++)
    {
        retMat[i] = (int**)malloc(2 * sizeof(int*));
 
        if (retMat[i] == NULL)
        {
            fprintf(stderr, "Out of memory");
            exit(0);
        }
        for (int j = 0; j < 2; j++)
        {
            retMat[i][j] = (int*)malloc(2 * sizeof(int));
            if (retMat[i][j] == NULL)
            {
                fprintf(stderr, "Out of memory");
                exit(0);
            }
        }
    }

    int k = 0;
    for (int i = 0; i < row; i = i + 2){
        for (int j = 0; j < col; j = j + 2){
            for (int x = 0; x < 2; x++)
                for (int y = 0; y < 2; y++)
                {

                    retMat[k][x][y] = mat[i+x][j+y];
                }
            k++;
        }
    }
    return retMat;
};


int **allocateMat(int **mat, int rows, int cols){
    mat = malloc(sizeof(int*)*rows);
    for(int i = 0; i< rows;i++)
        mat[i] = malloc(sizeof(int)*cols);
    return mat;
}

int checkLineMax(int num1, int num2)
{
    int max = ((num1 > MAX_LINE_LENGTH || num2 > MAX_LINE_LENGTH)) ? MAX_LINE_LENGTH * MAX_LINE_LENGTH : MAX_LINE_LENGTH;
    return max;
};