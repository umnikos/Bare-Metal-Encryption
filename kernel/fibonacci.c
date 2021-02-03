#include "prelude.h"

u32 fibonacci(u32 i) {
	if (i < 2) return 1;
	else return fibonacci(i-1) + fibonacci(i-2);
}
 
