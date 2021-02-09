#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define uptr uintptr_t

#define mem_barrier __sync_synchronize()

extern void crash(const char* msg);
extern void out_byte(u16 port, u8 data);
extern void out_word(u16 port, u16 data);
extern void out_dword(u16 port, u32 data);
extern uint8_t in_byte(u16 port);
extern u16 in_word(u16 port);
extern u32 in_dword(u16 port);
extern void waiting();
extern void nothing();
u32 fibonacci(u32 i);


void debug(const char* data);
