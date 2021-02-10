#pragma once

void serial_initialize();
void serial_send(const char* s);
void serial_receive(char* s, u32 size);
