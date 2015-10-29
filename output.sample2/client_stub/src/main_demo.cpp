#include <stdio.h>
#include "comp.h"

int main(int argc, char *argv[]) {
    printf("%.4lf / %.4lf = %.4lf\n", 4.0, 5.0, test1_divide(4, 5));

    int A[2][3] = {
        1, 2, 3,
        4, 5, 6
    };
    printf("transpose:\n");
    int B[3][2];
    test1_transpose(&A[0][0], 2, 3, &B[0][0], 3, 2);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {
            printf("%4d", B[i][j]);
        }
        printf("\n");
    }
    int C[] = {
        2,3,5,7,3,4,5,1,2
    };
    printf("sort:\n");
    test1_sort(sizeof(C) / sizeof(int), C);
    for (int i = 0; i < sizeof(C) / sizeof(int); ++i) {
        printf("%4d", C[i]);
    }
    printf("\n");
    return 0;
}
