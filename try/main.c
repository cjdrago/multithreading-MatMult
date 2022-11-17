// C Program to multiply two matrix using pthreads without
// use of global variables
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

int ***divMat(int row, int col, int **mat);
int **createMat(int row, int col);
void printMat(int row, int col, int **mat);
int **splitMult(int indexA, int indexB, int ***splitA, int ***splitB);
int **matPlus(int **matX, int **matY);

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

        for(int k=0; k,data->p/2; k++) {
            
        }
    };
    printf("dbasjkdas");
    

    

    // int x = data[0];
    // for (i = 1; i <= x; i++)
    //     k += data[i] * data[i + x];

    // int *p = (int *)malloc(sizeof(int));
    // *p = k;

    // // Used to terminate a thread and the return value is passed as a pointer
     
//Used to terminate a thread and the return value is passed as a pointer
    pthread_exit(data->C);
};




// Driver code
int main(int argc, char **argv)
{

    int n = atoi(argv[1]); // Rows matA = matB
    int p = atoi(argv[2]); // Columns matA
    int m = atoi(argv[3]); // Columns matB

    printf("\nThis system has %d processors configured and "
           "%d processors available.\n \n",
           get_nprocs_conf(), get_nprocs());

    int **matA= createMat(n, p);
    printMat(n, p, matA);

    int **matB = createMat(p, m);
    printMat(p, m, matB);

    int ***splitA = divMat(n, p, matA);
    int ***splitB = divMat(p, m, matB);

    int i, j, k;
    int max = (n * m)/4;

    // declaring array of threads of size n*m
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
    pthread_create(&threads[count++], NULL, sumMult,  (void *)&args);
    


    // printf("RESULTANT MATRIX IS :- \n");
    // for (i = 0; i < max; i++)
    // {
    //     void *k;

    //     // Joining all threads and collecting return value
        pthread_join(threads[count++], NULL);

    //     int *p = (int *)k;
    //     printf("%d ", *p);
    //     if ((i + 1) % m == 0)
    //         printf("\n");
    // };

    return 0;
};

int **splitMult(int indexA, int indexB, int ***splitA, int ***splitB){
    int **retMat;
    retMat = malloc(sizeof(int*)*2);

    for(int i =0; i<2; i++){
        retMat[i] = malloc(sizeof(int)*2);
    };

    retMat[0][0] = splitA[indexA][0][0]*splitB[indexB][0][0] + splitA[indexA][0][1]*splitB[indexB][1][0];

    retMat[0][1] = splitA[indexA][0][0]*splitB[indexB][1][0] + splitA[indexA][0][1]*splitB[indexB][1][1];

    retMat[1][0] = splitA[indexA][0][1]*splitB[indexB][0][0] + splitA[indexA][1][1]*splitB[indexB][1][0];

    retMat[1][1] = splitA[indexA][0][1]*splitB[indexB][1][0] + splitA[indexA][1][1]*splitB[indexB][1][1];

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

int **createMat(int row, int col)
{

    int **matrix;

    matrix = malloc(sizeof(int *) * row);

    for (int i = 0; i < col; i++)
        matrix[i] = malloc(sizeof(int) * col);

    for (int i = 0; i < row; i++)
        for (int j = 0; j < col; j++)
            matrix[i][j] = rand() % 10;

    return matrix;
};

void printMat(int row, int col, int **mat)
{
    printf("mat %d x %d: \n", row, col);
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
