#include <stdio.h>
#include <string.h>
#include "cs.h"

int main(int argc, char *argv[]) {
    printf("%d + %d = %d\n", 5, 6, add(5, 6));

    int D[] = {5,3,2,5,6,8,9};
    printf("max val=%d\n", max(sizeof(D) / sizeof(int), D));

    char *str = "hello, world, my name is yekeren";
    printf("there are %d words in %s\n", wc(str, strlen(str)), str);

    int A[3][2] = {
        1, 2,
        3, 4,
        5, 6
    };
    int B[2][3] = {
        1, 2, 3,
        4, 5, 6
    };
    int C[3][3] = {
        0
    };
    multiply(&A[0][0], 3, 2, &B[0][0], 2, 3, &C[0][0], 3, 3);
    printf("multiply:\n");
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("%4d", C[i][j]);
        }
        printf("\n");
    }
    return 0;
}
