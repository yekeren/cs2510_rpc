#include <stdio.h>
#include <string.h>
#include "cs.h"

int main(int argc, char *argv[]) {
    printf("%d + %d = %d\n", 4, 5, add(4, 5));
    int arr[] = { 1,2,3,4,5,6,7,8,9,0 };
    printf("max numer is %d\n", max(10, arr));
    char *str = "one two three four five six";
    printf("word count is %d\n", wc(str, strlen(str)));

    int A[2][3] = { 1,2,3,4,5,6 };
    int B[3][2] = { 1,2,3,4,5,6 };
    int C[2][2];
    multiply(&A[0][0], 2, 3, &B[0][0], 3, 2, &C[0][0], 2, 2);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            printf("%4d", C[i][j]);
        }
        printf("\n");
    }
    return 0;
}
