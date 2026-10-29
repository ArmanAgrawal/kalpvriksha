#include <stdio.h>
#include<stdlib.h>
#include<time.h>


void generateMatrix(int **mat,int *data,int n);
void printMatrix(int **mat,int n);
void swapslots(int **a,int **b);
void rotateMatrix(int **mat,int n);
void smoothMatrix(int **mat,int n);


int main(){
    int n;
    char buffer[100];
    char extra;
    printf("Enter matrix Size(2-10)");

    if (fgets(buffer, sizeof(buffer), stdin) == NULL ||
        sscanf(buffer, "%d %c", &n, &extra) != 1 || 
        n < 2 || n > 10) {
        printf("Invalid size Size must be an integer between 2 and 10.\n");
        return 1;
    }
    srand(time(NULL));

    int *data = malloc(n*n*sizeof(int));
    if (data == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    int **mat = malloc(n*n*sizeof(int *));

    if (mat == NULL) {
        printf("Memory allocation failed!\n");
        free(data);
        return 1;
    }

    generateMatrix(mat, data, n);

    printf("\nOriginal Randomly Generated Matrix:\n");
    printMatrix(mat, n);

    rotateMatrix(mat, n);
    printf("\nMatrix after 90° Clockwise Rotation:\n");
    printMatrix(mat, n);

    smoothMatrix(mat, n);
    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    printMatrix(mat, n);

    free(mat);
    free(data);
    return 0;
}

void generateMatrix(int **mat,int *data,int n){
    for(int i = 0;i<n*n;i++){
        *(data+i) = rand()%256;
        *(mat + i) = data + i;
    }
}


void printMatrix(int **mat,int n){
    for(int i = 0;i<n;i++){
        for(int j = 0;j<n;j++){
            printf("%4d",**(mat+i*n+j));
        }
        printf("\n");
    }
}

void swapslots(int **a,int **b){
    int *temp = *a;
    *a = *b;
    *b = temp;
}

void rotateMatrix(int **mat,int n){
    for(int i = 0;i<n;i++){
        for(int j = i+1;j<n;j++){
            swapslots(mat+i*n+j,mat+j*n+i);
        }
    }

    for(int i = 0;i<n;i++){
        for(int j = 0;j<n/2;j++){
            swapslots(mat+i*n+j,mat+i*n+(n-1-j));
        }
    }
}

void smoothMatrix(int **mat,int n){
    int *prev = calloc(n,sizeof(int));
    int *curr = calloc(n,sizeof(int));

    if (prev == NULL || curr == NULL) {
        printf("Memory allocation failed in smoothMatrix!\n");
        free(prev);
        free(curr);
        return;
    }

    for(int i = 0;i<n;i++)
    {
        for(int j = 0;j<n;j++){
            *(curr+j) = **(mat+i*n+j);
        }

        for(int j = 0;j<n;j++){
            int sum = 0;
            int count = 0;

            for(int r = i-1;r<=i+1;r++){
                if(r<0 || r>=n)continue;
                for(int c = j-1;c<=j+1;c++){
                    if(c<0 || c>=n)continue;

                    if(r==i-1){
                        sum+= *(prev+c);
                    }
                    else if(r==i){
                        sum+=*(curr+c);
                    }else{
                        sum+=**(mat+r*n+c);
                    }
                    count++;

                }
            }
            if(count>0){
                **(mat+i*n+j) = sum/count;
            }
        }
        int *t = prev;
        prev = curr;
        curr = t;
    }
    free(prev);
    free(curr);

}

