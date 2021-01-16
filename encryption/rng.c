#include "prelude.h"

#include <time.h>

// https://www.freecodecamp.org/news/random-number-generator/

i seed;

void init_rng() {
  // create seed from current time
  i timestamp = time(NULL);
  i millis = 0;
  while (time(NULL) == timestamp) ++millis;
  seed = millis * timestamp;

  // print debug info
  printf("timestamp: %d\n", timestamp);
  printf("millis: %d\n", millis);
  printf("seed: %d\n", seed);
}

// https://en.wikipedia.org/wiki/Xorshift#Example_implementation
// TODO - IMPLEMENT SOMETHING THAT ISN'T BAD.
i rng() {
  seed ^= seed << 13;
  seed ^= seed >> 17;
  seed ^= seed << 5;
  return seed;
}
