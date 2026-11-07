#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int main() 
{

    int n;
    scanf("%d", &n);
  	// Complete the code to print the pattern.
    int c = n-1;
    for(int i = 0;i<2*n-1;i++){
        for(int j = 0;j<2*n-1;j++){
            int x = abs(i-c);
            int y = abs(j-c);
            int maxi = 0;
            if(x>y){
                maxi = x;
            }else{
                maxi = y;
            }
            printf("%d",maxi+1);
            if(j<2*n-2){
                printf(" ");
            }
        }
        printf("\n");
    }
    return 0;
}
