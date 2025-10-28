#include <stdio.h>
#include <stdlib.h>

int compute_sum(int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) sum += i;
    return sum;
}

int main() {
    int n = 1000000;
    int result = compute_sum(n);
    printf("Sum up to %d = %d\n", n, result);
    return 0;
}
