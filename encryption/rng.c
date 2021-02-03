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
  char buf[64];
  write_out("timestamp: ");
  i_to_str(timestamp, buf, 64);
  write_out(buf);
  write_out("\n");
  write_out("millis: ");
  i_to_str(millis, buf, 64);
  write_out(buf);
  write_out("\n");
  write_out("seed: ");
  i_to_str(seed, buf, 64);
  write_out(buf);
  write_out("\n");
}

// https://en.wikipedia.org/wiki/Xorshift#Example_implementation
// TODO - IMPLEMENT SOMETHING THAT ISN'T BAD.
i rng() {
  seed ^= seed << 13;
  seed ^= seed >> 17;
  seed ^= seed << 5;
  return seed;
}
