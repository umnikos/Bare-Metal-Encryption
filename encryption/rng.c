#include "prelude.h"

#define SHOW_GENERATION() 0

// https://www.freecodecamp.org/news/random-number-generator/

i seed;

void init_rng() {
  // create seed from current time
  i timestamp = get_time();
  i millis = 0;
  while (get_time() == timestamp) ++millis;
  seed = millis * timestamp;

  // print debug info
#if SHOW_GENERATION()
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
#endif
}

// https://en.wikipedia.org/wiki/Xorshift#Example_implementation
// FIXME - XORSHIFT IS BAD
i rng() {
  seed ^= seed << 13;
  seed ^= seed >> 17;
  seed ^= seed << 5;
  return seed;
}
