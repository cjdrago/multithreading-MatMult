#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int MAX_LINE_LENGTH = 1000 * 1000;
int checkLineMax(int num1, int num2)
{
    int max = ((num1 > MAX_LINE_LENGTH || num2 > MAX_LINE_LENGTH)) ? MAX_LINE_LENGTH * MAX_LINE_LENGTH : MAX_LINE_LENGTH;
    return max;
};

int **allocateMat(int **mat, int rows, int cols){
    mat = malloc(sizeof(int*)*rows);
    for(int i = 0; i< rows;i++)
        mat[i] = malloc(sizeof(int)*cols);
    return mat;
}

int main(int argc, char **argv)
{
    FILE *textfile;
    char line[MAX_LINE_LENGTH];
    char firstLine[MAX_LINE_LENGTH];

    textfile = fopen(argv[1], "r");
    if (textfile == NULL)
        return 1;

    fscanf(textfile, "%[^\n]", firstLine);
    int n = atoi(strtok(firstLine, "\t"));
    int p = atoi(strtok(NULL, "\t"));
    int m = atoi(strtok(NULL, "\t"));

    MAX_LINE_LENGTH = checkLineMax(n, p);


    int **matA, **matB;
    matA = allocateMat(matA, n, p);
    matB = allocateMat(matB, p, m);

    int rowIndxA= 0, rowIndexB = 0, step = 0;
    while (fgets(line, MAX_LINE_LENGTH, textfile))
    {
        if(step > 0 && rowIndxA < n){
            for (int j = 0; j < p; j++){
                if (j == 0) matA[rowIndxA][j] = atoi(strtok(line, "\t"));
                else matA[rowIndxA][j] = atoi(strtok(NULL, "\t"));
            };
            rowIndxA++;
        }
        else if (step > n){
                for (int j = 0; j < m; j++){
                    if (j == 0) matB[rowIndexB][j] = atoi(strtok(line, "\t"));
                    else matB[rowIndexB][j] = atoi(strtok(NULL, "\t"));
                };
                rowIndexB++;        }
        step++;
    }

    for(int i = 0; i< n; i++){
        for(int j=0; j<p; j++)
            printf("%d ", matA[i][j]);
        printf("\n");
    }
        printf("\n");

    for(int i = 0; i< p; i++){
        for(int j=0; j< m; j++)
            printf("%d ", matB[i][j]);
        printf("\n");
    }
    fclose(textfile);
    return 0;
}
