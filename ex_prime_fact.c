#include <stdio.h>

void prime_factorization(int num) {
  printf("%d = ", num);
  int i = 2;
  while (i <= num) {
    if (num == i) {
      printf("%d\n", i);
      return;
    }
    if (num % i == 0) {
      printf("%d * ", i);
      num /= i;
    } else {
      i += 1;
    }
  }
}

int main() {
  prime_factorization(462);
}