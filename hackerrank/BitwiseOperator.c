#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
//Complete the following function.


void calculate_the_maximum(int n, int k) {
  //Write your code here.
  int maxAnd = 0;
  int maxOr =0;
  int maxXor = 0;
  for(int i = 1;i<=n;i++){
    for(int j = i+1;j<=n;j++){
        int andVal = i & j;
        int orVal = i|j;
        int xorVal = i^j;
        if(andVal<k && andVal>maxAnd){
            maxAnd = andVal;
        }
        if(orVal<k && orVal>maxOr){
            maxOr =orVal;
        }
        if(xorVal<k && xorVal>maxXor){
            maxXor=xorVal;
        }
    }
  }
  printf("%d\n",maxAnd);
  printf("%d\n",maxOr);
  printf("%d\n",maxXor);
}


int main() {
    int n, k;
  
    scanf("%d %d", &n, &k);
    calculate_the_maximum(n, k);
 
    return 0;
}
