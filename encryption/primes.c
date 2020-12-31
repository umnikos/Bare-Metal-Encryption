#include "prelude.h"
#include <stdlib.h>
#include <stdio.h>

i* primes = NULL;
i primes_count = 0;

static inline byte isPrime(i n) {
  for (i j=1 ; ; ++j) {
    if (n % primes[j] == 0) {
      return false;
    }
    if (primes[j]*primes[j] >= n) {
      return true;
    }
  }
}

void generatePrimes(i count) {
  if (count <= primes_count) return;
  if (count < 5) count = 5;
  primes = realloc(primes, sizeof(i) * count);
  primes[0] = 2;
  primes[1] = 3;
  primes_count = max(primes_count, 2);
  i n = primes[primes_count-1]+2;
  for (; primes_count<count; ++n, ++n) {
    if (isPrime(n)) {
      primes[primes_count] = n;
      ++primes_count;
    }
  }
}

int main() {
  generatePrimes(1000000);
  printf("done.\n");
  printf("%d\n", primes[999999]);
}
