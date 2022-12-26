#include <stdio.h>

int factorial(int n) {
  if (n == 0) { return 1; }
  return n * factorial(n-1);
}

int main() {
  int x = 5;
  int x_fact = factorial(5);
  printf("%d! = %d\n", x, x_fact);
}